#include "editor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "filedialog/ImGuiFileDialog.h"

#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "matrices.h"

#include "collisions.h"
#include "input.h"
#include "draw.h"
#include "timer_aux.h"
#include "bezier.h"
#include "gamedata.h"

//JSON
#include "vec_aux.h"//for vec3 json
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

//aux function
int find_index_string_vec(std::vector<const char*> vec, const char* str)
{
    for (int j = 0; j < (int)vec.size(); j++){
        if (!strcmp(vec[j],str)) {
            return j;
        }
    }
    return -1;
}

//quickfix aux function, rounds too small values to 0
//no setprecision modifier on json... fuuuuuck
glm::vec3 view_dir_epsilon(glm::vec3 view)
{
    glm::vec3 ret = view;
    const float eps = 1e-6;
    if (std::fabs(ret.x) < eps)
        ret.x = 0.0f;
    if (std::fabs(ret.y) < eps)
        ret.y = 0.0f;
    if (std::fabs(ret.z) < eps)
        ret.z = 0.0f;
    return ret;
}

void ConfirmDialog::open() {
    pending = true;
    result  = false;
}

void ConfirmDialog::render(const char* msg) {
    if (pending)
    {
        ImGui::OpenPopup("Confirm");
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        pending = false;
    }
    if (ImGui::BeginPopupModal("Confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(msg);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            result = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            result = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool StairbuilderData::checkValid()
{
    if (!vec3_finite(bounding.aabb_min) || !vec3_finite(bounding.aabb_max))
        return false;

    glm::vec3 sb_size = bounding.getSize();
    if (sb_size.x <= 0 || sb_size.y <= 0 || sb_size.z <= 0)
        return false;

    if (numSteps < 1)
        return false;

    if (thickness < 1 || thickness > numSteps)
        return false;

    return true;
}

glm::vec3 StairbuilderData::getStepIncrement()
{
    if (!checkValid())
        return glm::vec3(0.0f);

    return bounding.getSize() / (float)numSteps;
}

AABB StairbuilderData::getInitialStep()
{
    if (orientation == 0)
        return initial_vertical();
    else
        return initial_horizontal();
}

std::vector<ObstacleData> StairbuilderData::buildFull()
{
    std::vector<ObstacleData> ret;
    ret.clear();

    if (!checkValid())
        return ret;

    glm::vec3 step = getStepIncrement();

    AABB currentStep = getInitialStep();

    for (int i = 1; i <= numSteps; i++)
    {
        AABB new_aabb = currentStep;

        if (clamp)
        {
            new_aabb.aabb_min = ClosestPoint(new_aabb.aabb_min, bounding);
            new_aabb.aabb_max = ClosestPoint(new_aabb.aabb_max, bounding);
        }

        ObstacleData new_obdata;
        new_obdata.pos = new_aabb.getCenter();
        new_obdata.e_size = new_aabb.getSize();
        new_obdata.type = objType;
        ret.push_back(new_obdata);

        switch (direction)
        {
            case NORTH: // z+
                currentStep.aabb_min.z += step.z;
                currentStep.aabb_max.z += step.z;
                break;

            case SOUTH: // z-
                currentStep.aabb_min.z -= step.z;
                currentStep.aabb_max.z -= step.z;
                break;

            case EAST: // x-
                currentStep.aabb_min.x -= step.x;
                currentStep.aabb_max.x -= step.x;
                break;

            case WEST: // x+
                currentStep.aabb_min.x += step.x;
                currentStep.aabb_max.x += step.x;
                break;

            default:
                break;
        }

        currentStep.aabb_min.y += step.y;
        currentStep.aabb_max.y += step.y;
    }

    return ret;
}

AABB StairbuilderData::initial_vertical()
{
    glm::vec3 step = getStepIncrement();

    AABB currentStep;
    currentStep.aabb_min = bounding.aabb_min;
    currentStep.aabb_max = bounding.aabb_max;
    currentStep.aabb_max.y = currentStep.aabb_min.y + step.y;

    if (thickness > 1)
        currentStep.aabb_min.y -= (step.y * (float)(thickness - 1));

    switch (direction)
    {
        case NORTH: // z+
            currentStep.aabb_max.z = currentStep.aabb_min.z + step.z;
            break;

        case SOUTH: // z-
            currentStep.aabb_min.z = currentStep.aabb_max.z - step.z;
            break;

        case EAST: // x-
            currentStep.aabb_min.x = currentStep.aabb_max.x - step.x;
            break;

        case WEST: // x+
            currentStep.aabb_max.x = currentStep.aabb_min.x + step.x;
            break;

        default:
            break;
    }

    return currentStep;
}

AABB StairbuilderData::initial_horizontal()
{
    glm::vec3 step = getStepIncrement();

    AABB currentStep;
    currentStep.aabb_min = bounding.aabb_min;
    currentStep.aabb_max = bounding.aabb_max;
    currentStep.aabb_max.y = currentStep.aabb_min.y + step.y;

    switch (direction)
    {
        case NORTH: // z+
            currentStep.aabb_max.z = currentStep.aabb_min.z + (step.z * (float)thickness);
            break;

        case SOUTH: // z-
            currentStep.aabb_min.z = currentStep.aabb_max.z - (step.z * (float)thickness);
            break;

        case EAST: // x-
            currentStep.aabb_min.x = currentStep.aabb_max.x - (step.x * (float)thickness);
            break;

        case WEST: // x+
            currentStep.aabb_max.x = currentStep.aabb_min.x + (step.x * (float)thickness);
            break;

        default:
            break;
    }

    return currentStep;
}

void Editor::setEdited()
{
    if (!edited && ImGui::IsItemEdited())
        edited = true;
}

void Editor::updateDeltaTime()
{
    float currentTime = (float)glfwGetTime();
    deltaTime = currentTime - prevTime;
    prevTime  = currentTime;
}

void Editor::Init()
{
    player.init();

    player.DEBUG_noclip_fly =
    player.DEBUG_noclip_obs =
    player.DEBUG_noclip_lvl = true;

    newLevel();

    obstacleTypes.clear();
    enemyTypes.clear();

    int numObstacleTypes = g_GameData_Obstacles.size();
    obstacleTypes.reserve(numObstacleTypes);
    for (const auto& o : g_GameData_Obstacles) {
        obstacleTypes.push_back(const_cast<char*>(o.first.c_str()));
    }

    int numEnemyTypes = g_GameData_Enemies.size();
    enemyTypes.reserve(numEnemyTypes);
    for (const auto& e : g_GameData_Enemies) {
        enemyTypes.push_back(const_cast<char*>(e.first.c_str()));
    }

    prevTime = (float)glfwGetTime();
}

void Editor::newLevel()
{
    Level newlevel;
    level = newlevel;

    player.pos = level.player_pos = level.getLevelCenter();
    g_CameraTheta = g_CameraPhi = viewTheta = viewPhi = 0.0f;
    level.player_view = phi_theta_to_vec3(viewPhi, viewTheta);

    obstacleModal = enemyModal = stairbuilderModal = false;
    edited = false;

    //more stuff here?
}

void Editor::loadLevel(std::string filepath)
{
    // LOAD LEVEL DATA FROM FILE
    std::ifstream f_level(filepath);

    std::cout << "Loading level at " << filepath << "..." << std::endl;

    if ( f_level.fail() )
    {
        std::cout << "File failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        return;
    }

    try
    {
        json j_level = json::parse(f_level);
        level = j_level.get<Level>();
    }
    catch (const json::exception& e)
    {
        std::cout << "Invalid level file: " << e.what() << std::endl;
        f_level.close();
        return;
    }

    f_level.close();

    //set camera to player starting position
    player.pos = level.player_pos;
    g_CameraTheta = viewTheta = getTheta(level.player_view);
    g_CameraPhi   = viewPhi   = getPhi(level.player_view);

    obstacleModal = enemyModal = stairbuilderModal = false;
    edited = false;
}

void Editor::saveLevel(std::string filepath)
{
    //Validate level before saving
    //(ceiling cannot be below floor, dimensions cannot be <= 0, etc)
    std::vector<std::string> errors = level.checkValid();
    if (errors.size() > 0)
    {
        std::cout << "Level contains invalid data:" << std::endl;
        for (std::string &e : errors)
            std::cout << e << std::endl;
        return;
    }

    // SAVE LEVEL DATA TO FILE
    std::ofstream f_level(filepath);

    std::cout << "Saving level to " << filepath << "..." << std::endl;

    if ( f_level.fail() )
    {
        std::cout << "File failed to open" << std::endl;
        std::cout << "Error: " << strerror(errno) << std::endl;
        return;
    }

    json j_level = level;

    try
    {
        f_level << j_level.dump() << std::endl;
    }
    catch (const json::exception& e)
    {
        std::cout << "Failed to save level data: " << e.what() << std::endl;
        f_level.close();
        return;
    }

    f_level.close();

    obstacleModal = enemyModal = stairbuilderModal = false;
    edited = false;
}

void Editor::Update()
{
    updateDeltaTime();

    player.setView(g_CameraTheta, g_CameraPhi);
    player.update(deltaTime);
}

void Editor::DrawGUI(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // for the main window
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (edited)
        window_flags |= ImGuiWindowFlags_UnsavedDocument;

    // for the obstacle/enemy/etc tables
    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;
    ImVec2 table_outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 26);

    // for the obstacle/enemy/etc modals
    ImGuiWindowFlags modal_flags = 0;
    modal_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    modal_flags |= ImGuiWindowFlags_NoResize;
    modal_flags |= ImGuiWindowFlags_NoCollapse;
    // there's no flag for no close. ok i just gotta hope that doesn't break anything

    // for the file dialog
    ImGuiWindowFlags filedialog_flags = 0;
    filedialog_flags |= ImGuiWindowFlags_NoMove;
    filedialog_flags |= ImGuiWindowFlags_NoResize;
    filedialog_flags |= ImGuiWindowFlags_NoCollapse;
    ImVec2 filedialog_maxSize = ImVec2(390, 390);
    ImVec2 filedialog_minSize = filedialog_maxSize;

    // main window fills viewport
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    // enemy view yaw
    static float enemyYaw = 0.0f;

    // backup data for the modals
    static ObstacleData backup_obstacleData;
    static AABB backup_obstacleAABB;
    static EnemyData backup_enemyData;
    static StairbuilderData backup_stairbuilderData;

    // because i need an int* to use combobox
    static int stairbuilderDirection;

    // aabb toggle
    static int obstacle_useAABB = false;

    ImGui::Begin("CONTROLS", NULL, window_flags);

    ImGui::Separator();
    ImGui::Text("Camera position: %.3f %.3f %.3f", player.pos.x, player.pos.y, player.pos.z);
    ImGui::Text("Camera view dir: %.3f %.3f %.3f", player.view.x, player.view.y, player.view.z);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MainTab", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("File"))
        {
            static int fileaction = -1;
            static std::string savedPath = ".";

            static ConfirmDialog confirmFile;
            static bool doLevelOperation = false;

            if (ImGui::Button("New level"))
            {
                fileaction = -1;
                // if edited ask confirmation
                if (edited)
                    confirmFile.open();
                else
                    doLevelOperation = true;
            }

            if (ImGui::Button("Save level"))
            {
                fileaction = 0;
                doLevelOperation = true;
            }

            if (ImGui::Button("Load level"))
            {
                fileaction = 1;
                // if edited ask confirmation
                if (edited)
                    confirmFile.open();
                else
                    doLevelOperation = true;
            }

            // Confirm dialog
            confirmFile.render("Unsaved progress will be lost!");
            if (confirmFile.result)
            {
                confirmFile.result = false;
                doLevelOperation = true;
            }

            // If confirm/not edited, do action
            if (doLevelOperation)
            {
                doLevelOperation = false;
                IGFD::FileDialogConfig config;
                switch (fileaction) {
                    case -1:
                        // DO NEW LEVEL
                        newLevel();
                        break;

                    case 0:
                    case 1:
                        // OPEN SAVE/LOAD DIALOG
                        config.path = savedPath;
                        config.flags = ImGuiFileDialogFlags_Modal;

                        if (fileaction == 0) // confirm overwrite on save
                            config.flags |= ImGuiFileDialogFlags_ConfirmOverwrite;

                        ImGuiFileDialog::Instance()->OpenDialog("FILEDIALOG", "Choose File", ".json", config);
                        break;
                }
            }

            // File dialog
            if (fileaction != -1 && ImGuiFileDialog::Instance()->Display("FILEDIALOG", filedialog_flags, filedialog_minSize, filedialog_maxSize))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    savedPath = ImGuiFileDialog::Instance()->GetCurrentPath();

                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

                    switch (fileaction) {
                        case 0:
                            saveLevel(filePathName);
                            break;

                        case 1:
                            loadLevel(filePathName);
                            break;
                    }
                }

                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::Separator();

            if (ImGui::Button("Validate level data"))
            {
                std::vector<std::string> errors = level.checkValid();
                if (errors.size() > 0)
                {
                    std::cout << "Level contains invalid data:" << std::endl;
                    for (std::string &e : errors)
                        std::cout << e << std::endl;
                }
                else
                    std::cout << "Level is valid" << std::endl;
            }

            if (ImGui::Button("Fix invalid data"))
            {
                level.setValidDefaults();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Level Info"))
        {
            //backup data
            static std::string backup_level_name = level.level_name;
            static glm::vec3 backup_player_pos = level.player_pos;
            static float backup_width   = level.levelWidth;
            static float backup_length  = level.levelLength;
            static float backup_floor   = level.levelFloor;
            static float backup_ceiling = level.levelCeiling;

            ImGui::InputText("Level name", &level.level_name); setEdited();
            //VALIDATE IS NOT EMPTY
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (level.level_name.empty())
                    level.level_name = backup_level_name;
                else
                    backup_level_name = level.level_name;
            }

            ImGui::Separator();

            ImGui::InputFloat3("Starting position", glm::value_ptr(level.player_pos)); setEdited();
            //VALIDATE IS NOT NAN
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!vec3_finite(level.player_pos))
                    level.player_pos = backup_player_pos;
                else
                    backup_player_pos = level.player_pos;
            }

            // i don't think you can even set this to nan from the GUI?
            // if you can then congratulations, but the level won't save anyways
            ImGui::SliderAngle("Starting yaw", &viewTheta); setEdited();
            ImGui::SliderAngle("Starting pitch", &viewPhi); setEdited();
            level.player_view = phi_theta_to_vec3(viewPhi, viewTheta);
            //precision quickfix
            level.player_view = view_dir_epsilon(level.player_view);

            ImGui::Separator();

            //FOR EACH, VALIDATE IS NOT DEGENERATE

            ImGui::InputFloat("Width",    &level.levelWidth,      0.25f, 1.0f, "%.2f"); setEdited();
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!level.checkValidBoundaries())
                    level.levelWidth = backup_width;
                else
                    backup_width = level.levelWidth;
            }

            ImGui::InputFloat("Length",   &level.levelLength,     0.25f, 1.0f, "%.2f"); setEdited();
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!level.checkValidBoundaries())
                    level.levelLength = backup_length;
                else
                    backup_length = level.levelLength;
            }

            ImGui::InputFloat("Floor",    &level.levelFloor,      0.25f, 1.0f, "%.2f"); setEdited();
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!level.checkValidBoundaries())
                    level.levelFloor = backup_floor;
                else
                    backup_floor = level.levelFloor;
            }

            ImGui::InputFloat("Ceiling",  &level.levelCeiling,    0.25f, 1.0f, "%.2f"); setEdited();
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!level.checkValidBoundaries())
                    level.levelCeiling = backup_ceiling;
                else
                    backup_ceiling = level.levelCeiling;
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Obstacles"))
        {
            if (ImGui::Button("New obstacle"))
            {
                obstacleModal = true;
                obstacleIndex = -1;

                ObstacleData new_obdata;
                new_obdata.pos = level.getLevelCenter();
                new_obdata.e_size = glm::vec3(1.0f,1.0f,1.0f);
                new_obdata.type = obstacleTypes[0];

                obstacleData = new_obdata;
                obstacleTypeIndex = 0;

                backup_obstacleData = obstacleData;
                backup_obstacleAABB = AABB::from_pos_size(obstacleData.pos, obstacleData.e_size);
                obstacle_useAABB = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Open stair builder"))
            {
                stairbuilderModal = true;

                StairbuilderData new_sbdata;
                new_sbdata.bounding     = AABB::from_pos_size(level.getLevelCenter(), glm::vec3(1.0f,1.0f,1.0f));
                new_sbdata.direction    = NORTH;
                new_sbdata.orientation  = 0;
                new_sbdata.numSteps     = 4;
                new_sbdata.thickness    = 4;
                new_sbdata.clamp        = true;
                new_sbdata.objType      = obstacleTypes[0];

                stairbuilderData = new_sbdata;
                stairbuilderTypeIndex = 0;

                backup_stairbuilderData = stairbuilderData;
                stairbuilderDirection = NORTH;
            }

            ImGui::Separator();

            int obstacleCount = level.obstacles.size();
            static ConfirmDialog confirmDelete;
            static int indexDelete = -1;

            ImGui::Text("Obstacle count: %d", obstacleCount);
            if (obstacleCount > 0 && ImGui::BeginTable("Obstacles", 2, table_flags, table_outer_size))
            {
                for (int i = 0; i < obstacleCount; i++)
                {
                    ImGui::TableNextRow();
                    ImGui::PushID(i);

                    ImGui::TableNextColumn();
                    ImGui::Text("#%d: %s", i, level.obstacles[i].type.c_str());

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Edit"))
                    {
                        obstacleModal = true;
                        obstacleIndex = i; // edit object
                        obstacleData = level.obstacles[i];
                        obstacleTypeIndex = find_index_string_vec(obstacleTypes, obstacleData.type.c_str());

                        backup_obstacleData = obstacleData;
                        backup_obstacleAABB = AABB::from_pos_size(obstacleData.pos, obstacleData.e_size);
                        obstacle_useAABB = false;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Clone"))
                    {
                        obstacleModal = true;
                        obstacleIndex = -1; // new object
                        obstacleData = level.obstacles[i];
                        obstacleTypeIndex = find_index_string_vec(obstacleTypes, obstacleData.type.c_str());

                        backup_obstacleData = obstacleData;
                        backup_obstacleAABB = AABB::from_pos_size(obstacleData.pos, obstacleData.e_size);
                        obstacle_useAABB = false;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Delete"))
                    {
                        indexDelete = i;
                        confirmDelete.open();
                    }

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            confirmDelete.render("Delete this obstacle?");
            if (confirmDelete.result)
            {
                confirmDelete.result = false;
                level.obstacles.erase(level.obstacles.begin()+indexDelete);
                edited = true;
                indexDelete = -1;
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Enemies"))
        {
            if (ImGui::Button("New enemy"))
            {
                enemyModal = true;
                enemyIndex = -1;

                EnemyData new_enemydata;
                new_enemydata.pos = level.getLevelCenter();
                new_enemydata.view = glm::vec3(0.0f,0.0f,1.0f);
                new_enemydata.type = enemyTypes[0];
                enemyTypeIndex = 0;

                enemyData = new_enemydata;

                backup_enemyData = enemyData;
                enemyYaw = 0.0f;
            }
            ImGui::Separator();

            int enemyCount = level.enemies.size();
            static ConfirmDialog confirmDelete;
            static int indexDelete = -1;

            ImGui::Text("Enemy count: %d", enemyCount);
            if (enemyCount > 0 && ImGui::BeginTable("Enemies", 2, table_flags, table_outer_size))
            {
                for (int i = 0; i < enemyCount; i++)
                {
                    ImGui::TableNextRow();
                    ImGui::PushID(i);

                    ImGui::TableNextColumn();
                    ImGui::Text("#%d: %s", i, level.enemies[i].type.c_str());

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Edit"))
                    {
                        enemyModal = true;
                        enemyIndex = i; // edit object
                        enemyData = level.enemies[i];
                        enemyTypeIndex = find_index_string_vec(enemyTypes, enemyData.type.c_str());

                        backup_enemyData = enemyData;
                        enemyYaw = getTheta(enemyData.view);
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Clone"))
                    {
                        enemyModal = true;
                        enemyIndex = -1; // new object
                        enemyData = level.enemies[i];
                        enemyTypeIndex = find_index_string_vec(enemyTypes, enemyData.type.c_str());

                        backup_enemyData = enemyData;
                        enemyYaw = getTheta(enemyData.view);
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Delete"))
                    {
                        indexDelete = i;
                        confirmDelete.open();
                    }

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            confirmDelete.render("Delete this enemy?");
            if (confirmDelete.result)
            {
                confirmDelete.result = false;
                level.enemies.erase(level.enemies.begin()+indexDelete);
                edited = true;
                indexDelete = -1;
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Separator();

    if (obstacleModal)
    {
        ImGui::Begin("Obstacle Data", &obstacleModal, modal_flags);
        ImGui::Separator();

        ImGui::RadioButton("Position/Size", &obstacle_useAABB, false);
        ImGui::SameLine();
        ImGui::RadioButton("AABB", &obstacle_useAABB, true);

        ImGui::Separator();

        //edit AABB directly
        AABB obstacleAABB = AABB::from_pos_size(obstacleData.pos, obstacleData.e_size);

        if (obstacle_useAABB)
        {
            ImGui::InputFloat3("Min point", glm::value_ptr(obstacleAABB.aabb_min));
            //VALIDATE
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                glm::vec3 e_size = obstacleAABB.getSize();
                if (!vec3_finite(obstacleAABB.aabb_min))
                    obstacleAABB = backup_obstacleAABB;
                else if (e_size.x <= 0 || e_size.y <= 0 || e_size.z <= 0)
                    obstacleAABB = backup_obstacleAABB;
                else
                    backup_obstacleAABB = obstacleAABB;
            }

            ImGui::InputFloat3("Max point", glm::value_ptr(obstacleAABB.aabb_max));
            //VALIDATE
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                glm::vec3 e_size = obstacleAABB.getSize();
                if (!vec3_finite(obstacleAABB.aabb_max))
                    obstacleAABB = backup_obstacleAABB;
                else if (e_size.x <= 0 || e_size.y <= 0 || e_size.z <= 0)
                    obstacleAABB = backup_obstacleAABB;
                else
                    backup_obstacleAABB = obstacleAABB;
            }

            //set obstacle data
            obstacleData.pos    = obstacleAABB.getCenter();
            obstacleData.e_size = obstacleAABB.getSize();
        }
        else
        {
            ImGui::InputFloat3("Position", glm::value_ptr(obstacleData.pos));
            //VALIDATE IS NOT NAN
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!obstacleData.checkValid())
                    obstacleData.pos = backup_obstacleData.pos;
                else
                    backup_obstacleData.pos = obstacleData.pos;
            }

            ImGui::InputFloat3("Size", glm::value_ptr(obstacleData.e_size));
            //VALIDATE IS NOT DEGENERATE
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                if (!obstacleData.checkValid())
                    obstacleData.e_size = backup_obstacleData.e_size;
                else
                    backup_obstacleData.e_size = obstacleData.e_size;
            }
        }

        if (ImGui::Combo("Type", &obstacleTypeIndex, obstacleTypes.data(), (int)obstacleTypes.size()))
            obstacleData.type = std::string(obstacleTypes[obstacleTypeIndex]);

        ImGui::Separator();

        if (ImGui::Button("Save"))
        {
            if (obstacleIndex == -1)
            {
                //push back
                level.obstacles.push_back(obstacleData);
            }
            else
            {
                //update
                level.obstacles[obstacleIndex] = obstacleData;
            }

            edited = true;

            obstacleModal = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            obstacleModal = false;
        }

        ImGui::End();
    }

    if (enemyModal)
    {
        ImGui::Begin("Enemy Data", &enemyModal, modal_flags);
        ImGui::Separator();

        ImGui::InputFloat3("Position", glm::value_ptr(enemyData.pos));
        //VALIDATE IS NOT NAN
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            if (!enemyData.checkValid())
                enemyData.pos = backup_enemyData.pos;
            else
                backup_enemyData.pos = enemyData.pos;
        }

        // enemy view dir
        // just yaw for now
        // i don't think you can even set this to nan from the GUI?
        // if you can then congratulations, but the level won't save anyways
        ImGui::SliderAngle("View yaw", &enemyYaw);
        enemyData.view = phi_theta_to_vec3(0.0f, enemyYaw);
        //precision quickfix
        enemyData.view = view_dir_epsilon(enemyData.view);

        if (ImGui::Combo("Type", &enemyTypeIndex, enemyTypes.data(), (int)enemyTypes.size()))
            enemyData.type = std::string(enemyTypes[enemyTypeIndex]);

        ImGui::Separator();

        if (ImGui::Button("Save"))
        {
            if (enemyIndex == -1)
            {
                //push back
                level.enemies.push_back(enemyData);
            }
            else
            {
                //update
                level.enemies[enemyIndex] = enemyData;
            }

            edited = true;

            enemyModal = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            enemyModal = false;
        }

        ImGui::End();
    }

    if (stairbuilderModal)
    {
        ImGui::Begin("Stair Builder", &stairbuilderModal, modal_flags);
        ImGui::Separator();

        ImGui::InputFloat3("Min point", glm::value_ptr(stairbuilderData.bounding.aabb_min));
        //VALIDATE
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            if (!stairbuilderData.checkValid())
                stairbuilderData.bounding = backup_stairbuilderData.bounding;
            else
                backup_stairbuilderData.bounding = stairbuilderData.bounding;
        }

        ImGui::InputFloat3("Max point", glm::value_ptr(stairbuilderData.bounding.aabb_max));
        //VALIDATE
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            if (!stairbuilderData.checkValid())
                stairbuilderData.bounding = backup_stairbuilderData.bounding;
            else
                backup_stairbuilderData.bounding = stairbuilderData.bounding;
        }

        static std::vector<const char*> directionNames = {"North (+Z)", "South (-Z)", "East (-X)", "West (+X)"};
        if (ImGui::Combo("Direction", &stairbuilderDirection, directionNames.data(), (int)directionNames.size()))
            stairbuilderData.direction = static_cast<CardinalDirection>(stairbuilderDirection);

        if (ImGui::Combo("Obstacle type", &stairbuilderTypeIndex, obstacleTypes.data(), (int)obstacleTypes.size()))
            stairbuilderData.objType = std::string(obstacleTypes[stairbuilderTypeIndex]);

        ImGui::RadioButton("Vertical", &stairbuilderData.orientation, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Horizontal", &stairbuilderData.orientation, 1);

        int thickness_diff = stairbuilderData.numSteps - stairbuilderData.thickness;
        if (ImGui::SliderInt("Steps", &stairbuilderData.numSteps, 1, 10))
        {
            //cases:
            // th = ns: th = ns
            // th < ns: th = min(th, ns)
            // th > ns: th = ns
            if (thickness_diff > 0)
                stairbuilderData.thickness = std::min(stairbuilderData.thickness, stairbuilderData.numSteps);
            else
                stairbuilderData.thickness = stairbuilderData.numSteps;
        }
        //VALIDATE
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            if (!stairbuilderData.checkValid())
                stairbuilderData.numSteps = backup_stairbuilderData.numSteps;
            else
            {
                backup_stairbuilderData.numSteps = stairbuilderData.numSteps;
                backup_stairbuilderData.thickness = stairbuilderData.thickness;
            }
        }

        ImGui::SliderInt("Thickness", &stairbuilderData.thickness, 1, stairbuilderData.numSteps);
        //VALIDATE
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            if (!stairbuilderData.checkValid())
                stairbuilderData.thickness = backup_stairbuilderData.thickness;
            else
                backup_stairbuilderData.thickness = stairbuilderData.thickness;
        }

        ImGui::Checkbox("Clamp to bounding box", &stairbuilderData.clamp);

        glm::vec3 stepSize = stairbuilderData.getInitialStep().getSize();
        ImGui::Text("Obstacle size: %.3f, %.3f, %.3f", stepSize.x, stepSize.y, stepSize.z);

        float stepHeight = stairbuilderData.getStepIncrement().y;
        ImGui::Text("Step height: %.3f", stepHeight);
        if (stepHeight <= player.step_limit)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Player can autostep");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Player cannot autostep");

        ImGui::Separator();

        if (ImGui::Button("Save"))
        {
            //add
            auto stair = stairbuilderData.buildFull();
            level.obstacles.insert(level.obstacles.end(), stair.begin(), stair.end());

            edited = true;

            stairbuilderModal = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            stairbuilderModal = false;
        }

        ImGui::End();
    }

    ImGui::End();

    //RENDER GUI
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //reset background AND depth buffer
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void Editor::DrawLevel(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(g_GpuProgramID);

    glm::vec4 camera_pos  = Ponto(player.pos);
    glm::vec4 view_vector = Vetor(player.view);
    glm::vec4 up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f);

    glm::mat4 view = Matrix_Camera_View(camera_pos, view_vector, up_vector);

    float nearplane = -0.1f;   // Posição do "near plane"
    float farplane  = -100.0f; // Posição do "far plane"

    float camera_fov = 3.141592 / 2.0f; //90graus
    glm::mat4 projection = Matrix_Perspective(camera_fov, g_ScreenRatio, nearplane, farplane);

    setViewMatrix(view);
    setProjectionMatrix(projection);

    // reseta repetição de texturas
    resetTextureRepeat(); // this FAILS the first frame it runs. god only knows why

    // reseta alpha stuff
    // these FAIL the first frame they run. god only knows why
    resetAlphaMask();
    resetAlphaValue();

    //reset misc flags
    setIgnoreLighting(false);
    setUseGouraud(false);
    setUseSphericalUV(false);

    drawFloor(level);
    drawWall(level, NORTH);
    drawWall(level, SOUTH);
    drawWall(level, EAST);
    drawWall(level, WEST);

    //TRANSFORM ALL LEVEL DATA INTO DRAWABLE DATA
    std::vector<Obstacle> obstacles;
    std::vector<Enemy> enemies;

    for (ObstacleData& ob : level.obstacles)
    {
        obstacles.push_back(ob.buildObstacle());
    }

    for (EnemyData& enemy : level.enemies)
    {
        enemies.push_back(enemy.buildEnemy());
    }

    // DRAW DATA BEING EDITED
    if (obstacleModal)
    {
        Obstacle ob = obstacleData.buildObstacle();
        ob.editing = true;
        obstacles.push_back(ob);
    }

    if (enemyModal)
    {
        Enemy en = enemyData.buildEnemy();
        en.editing = true;
        enemies.push_back(en);
    }

    if (stairbuilderModal)
    {
        for (auto& obdata : stairbuilderData.buildFull())
        {
            Obstacle ob = obdata.buildObstacle();
            ob.editing = true;
            obstacles.push_back(ob);
        }
    }

    //draws all entities (obstacles, projectiles, enemies)
    //transparent entities are drawn after opaque entities,
    //sorted by distance from camera (works well enough...)
    std::multimap<float,std::shared_ptr<Entity>> trans_entities;
    std::multimap<float,std::shared_ptr<Entity>>::reverse_iterator it;

    for (int i = 0; i < (int)obstacles.size(); i++)
    {
        auto item = obstacles[i];

        //if is the existing item currently being edited, don't draw it, draw the edit data only
        if (obstacleModal && i == obstacleIndex)
            continue;

        //IF item.drawdata can have transparency, add to trans_entities[distance] to be drawn later, else draw now
        //also if is editing data
        if (item.editing || g_GameData_DrawData[item.dd_key].may_use_alpha)
        {
            glm::vec3 closest_point = ClosestPoint(toVec3(camera_pos), item.getHitbox());
            float distance = glm::length(toVec3(camera_pos) - closest_point);
            trans_entities.insert({distance, std::make_shared<Obstacle>(item)});
        }
        else
            item.draw();
    }
    for (int i = 0; i < (int)enemies.size(); i++)
    {
        auto item = enemies[i];

        //if is the existing item currently being edited, don't draw it, draw the edit data only
        if (enemyModal && i == enemyIndex)
            continue;

        //IF item.drawdata can have transparency, add to trans_entities[distance] to be drawn later, else draw now
        //also if is editing data
        if (item.editing || g_GameData_DrawData[item.dd_key].may_use_alpha)
        {
            glm::vec3 closest_point = ClosestPoint(toVec3(camera_pos), item.getHitbox());
            float distance = glm::length(toVec3(camera_pos) - closest_point);
            trans_entities.insert({distance, std::make_shared<Enemy>(item)});
        }
        else
            item.draw();
    }
    for (it = trans_entities.rbegin(); it != trans_entities.rend(); ++it)
    {
        auto item = it->second;

        if (item->editing){
            drawAABB(item->getHitbox());
            setAlphaValue(0.333f);
        }

        item->draw();
        resetAlphaValue();//safety
    }

    if (stairbuilderModal)
        drawAABB(stairbuilderData.bounding);

    //draw axes in middle of level
    drawAxes(level.getLevelCenter(),glm::vec4(1.0f,0.0f,0.0f,0.0f),glm::vec4(0.0f,1.0f,0.0f,0.0f),glm::vec4(0.0f,0.0f,1.0f,0.0f));

    //draw starting player info
    Player startingpos;
    startingpos.init();
    startingpos.pos = level.player_pos;
    startingpos.view = level.player_view;

    drawAABB(startingpos.getHitbox());

    startingpos.pos.y += startingpos.neck;
    drawPoint(startingpos.pos);
    for (int i = 5; i <= 10; i++)
        drawPoint(startingpos.pos + (startingpos.view * (i * 0.1f)));

    /* Currently unused
    // Os objetos a seguir sempre serão desenhados na frente; desativa o z-buffer
    glDisable(GL_DEPTH_TEST);

    // Últimas coisas são desenhadas diretamente em NDC
    // Desativa matrizes de view e projeção
    view       = Matrix_Identity();
    projection = Matrix_Identity();

    setViewMatrix(view);
    setProjectionMatrix(projection);

    glEnable(GL_DEPTH_TEST);
    */

    TextRendering_ShowFramesPerSecond(window);

    glfwSwapBuffers(window);
}

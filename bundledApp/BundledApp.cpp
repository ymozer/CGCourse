#include "BundledApp.hpp"
#include "Chapter01-Window/Chapter01.hpp"
#include "Chapter02-Point/Chapter02.hpp"
#include "Chapter03-Triangle/Chapter03.hpp"
#include "Chapter04-Colors/Chapter04.hpp"
#include "Chapter05-Square/Chapter05.hpp"
#include "Chapter06-Uniforms/Chapter06.hpp"
#include "Chapter07-IndexBuffer/Chapter07.hpp"
#include "Chapter08-Textures/Chapter08.hpp"
#include "Chapter09-Transformations/Chapter09.hpp"
#include "Chapter10-TransformationsCube/Chapter10.hpp"
#include "Chapter11-Camera/Chapter11.hpp"
#include "Chapter12-FaceCulling/Chapter12.hpp"
#include "Chapter13-LightingPhong/Chapter13.hpp"
#include "Chapter14-LightingBlinnPhong/Chapter14.hpp"
#include "Chapter15-Materials/Chapter15.hpp"
#include "Chapter16-SpecularMapping/Chapter16.hpp"
#include "Chapter17-LightTypes/Chapter17.hpp"
#include "Chapter18-MultipleLights/Chapter18.hpp"



void BundledApp::setup()
{
    m_AvailableChapters.push_back({"Chapter 1: The Basics", []() { return std::make_unique<Chapter01_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 2: The Point", []() { return std::make_unique<Chapter02_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 3: The Triangle", []() { return std::make_unique<Chapter03_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 4: Colors", []() { return std::make_unique<Chapter04_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 5: Square", []() { return std::make_unique<Chapter05_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 6: Uniforms", []() { return std::make_unique<Chapter06_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 7: Index Buffer", []() { return std::make_unique<Chapter07_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 8: Textures", []() { return std::make_unique<Chapter08_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 9: Transformations", []() { return std::make_unique<Chapter09_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 10: Transform Cube", []() { return std::make_unique<Chapter10_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 11: Camera", []() { return std::make_unique<Chapter11_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 12: Face Culling", []() { return std::make_unique<Chapter12_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 13: Lighting & Phong Shading", []() { return std::make_unique<Chapter13_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 14: Lighting & Blinn-Phong Shading", []() { return std::make_unique<Chapter14_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 15: Materials", []() { return std::make_unique<Chapter15_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 16: Specular Mapping", []() { return std::make_unique<Chapter16_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 17: Light Types", []() { return std::make_unique<Chapter17_Application>(); }});
    m_AvailableChapters.push_back({"Chapter 18: Multiple Lights", []() { return std::make_unique<Chapter18_Application>(); }});

    // ... add all other chapters

    // Start with no chapter selected.
    switchChapter(-1);
}

void BundledApp::shutdown()
{
    // Make sure the last chapter is shut down properly.
    if (m_CurrentChapter) {
        m_CurrentChapter->shutdown();
        m_CurrentChapter.reset();
    }
}

void BundledApp::switchChapter(int chapterIndex)
{
    // 1. Shut down the old chapter if one is running.
    if (m_CurrentChapter) {
        m_CurrentChapter->shutdown();
        m_CurrentChapter.reset(); // This deletes the old chapter object.
    }

    m_CurrentChapterIndex = chapterIndex;

    // 2. Create and set up the new chapter.
    if (m_CurrentChapterIndex >= 0 && m_CurrentChapterIndex < m_AvailableChapters.size()) {
        m_CurrentChapter = m_AvailableChapters[m_CurrentChapterIndex].second();
        m_CurrentChapter->setup();
    }
}

void BundledApp::update(float deltaTime)
{
    if (m_CurrentChapter) {
        m_CurrentChapter->update(deltaTime);
    }
}

void BundledApp::render()
{
    // If a chapter is active, call its render function.
    if (m_CurrentChapter) {
        m_CurrentChapter->render();
    } else {
        // If no chapter is selected, just clear to a default color.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void BundledApp::renderChapterUI()
{
    ImGui::Begin("Chapter Selection");

    for (int i = 0; i < m_AvailableChapters.size(); ++i) {
        if (ImGui::Button(m_AvailableChapters[i].first.c_str())) {
            if (i != m_CurrentChapterIndex) {
                switchChapter(i);
            }
        }
    }
    
    if (m_CurrentChapterIndex == -1) {
        ImGui::Text("\nPlease select a chapter to begin.");
    }
    
    ImGui::End();

    // If a chapter is active, render its specific UI too.
    if (m_CurrentChapter) {
        m_CurrentChapter->renderChapterUI();
    }
}

void BundledApp::handleInput(float deltaTime)
{
    // if (m_CurrentChapter) { m_CurrentChapter->handleInput(deltaTime); }
}
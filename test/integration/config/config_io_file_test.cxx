///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, Arthur N. Klassen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////
//
// 2017.04.27 - First version
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
///////////////////////////////////////////////////////////////////////////
//
// config_io_file_test.hxx -- unit tests for file I/O of configuration
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <config_io.hxx>
#include <config_paths.hxx>
#include <file_handle.hxx>
#include <file_path.hxx>
#include <file_system_path.hxx>
#include <iostream>

using namespace std;
using namespace testing;
using namespace ansak;
using namespace ansak::config;

namespace {

class OneShotContext {
public:
    bool directoryExists();
    void placeSettings();
    void removeSettings();

private:
    bool m_contextSet = false;
    bool m_checked = false;
    bool m_exists = false;
#if defined(WIN32)
#else
    FileSystemPath m_testSettingsPath;
#endif
};

bool OneShotContext::directoryExists()
{
    if (!m_contextSet)
    {
        setContext("ansak", "integrationTest");
        m_contextSet = true;
    }

    if (!m_checked)
    {
        m_checked = true;
#if defined(WIN32)
#else
        auto userSettingsPath = getUserConfigFilePath();
        auto userSettingsDir = FileSystemPath(userSettingsPath.parent());
        m_exists = userSettingsDir.exists() && userSettingsDir.isDir();
        if (m_exists)
        {
            m_testSettingsPath = userSettingsPath;
        }
        else
        {
            cout << "Config file tests will not be run because the $HOME/.ansak "
                 << "directory does not exist." << endl;
        }
#endif
    }
    return m_exists;
}

void OneShotContext::placeSettings()
{
    if (directoryExists())
    {
#if defined(WIN32)
#else
        auto handle = FileHandle::create(m_testSettingsPath, FileHandle::kOpenIfThere);
        const char settingsText[] = "pi=3.1428571429\n"
                                    "thePoint=56,78\n"
                                    "theBigBox=(20,30),(800,600)\n"
                                    "theSmallBox=(40,50),(60,20)\n"
                                    "theTruth=true\n"
                                    "theLie=This statement might not be true.\n";
        handle.write(settingsText, sizeof(settingsText) - 1);
#endif
    }
}

void OneShotContext::removeSettings()
{
    if (directoryExists())
    {
#if defined(WIN32)
#else
        m_testSettingsPath.remove();
#endif
    }
}


OneShotContext oneShot;

}

#if defined(WIN32)



#else

TEST(ConfigIOFileTest, testTheLinuxSettingsFiles)
{
    Config userSettings;
    if (oneShot.directoryExists())
    {
        try
        {
            oneShot.placeSettings();
            userSettings = getUserConfig();

            double pi;
            Point thePoint;
            Rect theBig, theSmall;
            bool theTruth;
            string theLie;
            userSettings.get("pi", pi);
            userSettings.get("thePoint", thePoint);
            userSettings.get("theBigBox", theBig);
            userSettings.get("theSmallBox", theSmall);
            userSettings.get("theTruth", theTruth);
            userSettings.get("theLie", theLie);

            userSettings.put("theTruth", false);
            EXPECT_TRUE(saveUserConfig(userSettings));
            userSettings.put("theTruth", true);
            Config otherSettings(getUserConfig());
            EXPECT_NE(userSettings, otherSettings);
            otherSettings.put("theTruth", true);
            EXPECT_EQ(userSettings, otherSettings);
        }
        catch( ... )
        {
        }
        oneShot.removeSettings();
        EXPECT_EQ(Config(), getUserConfig());
    }
    else
    {
        EXPECT_FALSE(saveUserConfig(userSettings));
    }
    EXPECT_EQ(Config(), getSystemConfig());
}

#endif

TEST(ConfigIOFileTest, testFileNotThere)
{
    FilePath thisDir(FilePath(__FILE__).parent());
    FilePath notThere(thisDir.child("peek-a-boo.rc"));
    try
    {
        getFileIfThere(notThere);
        ASSERT_FALSE(true);
    }
    catch (ConfigFileNotThere& e)
    {
        cout << "Caught exception: " << e.what() << endl;
    }
    EXPECT_EQ(Config(), getConfig(notThere));
}

TEST(ConfigIOFileTest, testBadWrites)
{
    Config settings;

    settings.put("pi", 3.14159265);
    settings.put("thePoint", toPoint(3, 5));
    settings.put("theBigBox", toRect(0, 0, 800, 600));
    settings.put("theSmallBox", toRect(100, 15, 80, 40));
    settings.put("theTruth", true);
    settings.put("theLie", "This sentence is not true");

    EXPECT_FALSE(writeConfig(FilePath::invalidPath(), settings));
    FilePath thisDir(FilePath(__FILE__).parent());
    EXPECT_FALSE(writeConfig(thisDir, settings));
    FilePath noParent(thisDir.child("jabberwock").child("snarky").child("boojum"));
    EXPECT_FALSE(writeConfig(noParent, settings));
}
project "Titan"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    -- Sets RuntimLibrary to MultiThreaded (non DLL version for static linking)
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src\\**.cpp",
        "include\\**.h"
    }

     links {
        "Glad",
        "GLFW",
        "stbs",
        "spdlog",
        "opengl32.lib",
        "toolkit",
        "ImGui",
        "imagehlp.lib",
        "%{wks.location}\\dependencies\\fmod\\fmod64.lib",
        "%{wks.location}\\dependencies\\gzip\\zlib.lib",
        "tinyGLTF"
    }

    includedirs {
        "%{prj.location}\\include",
        "%{wks.location}\\dependencies\\spdlog\\include",
        "%{wks.location}\\dependencies\\GLM\\include",
        "%{wks.location}\\dependencies\\glad\\include",
        "%{wks.location}\\dependencies\\glfw3\\include",
        "%{wks.location}\\dependencies\\entt",
        "%{wks.location}\\dependencies\\stbs",
        "%{wks.location}\\dependencies\\imgui",
        "%{wks.location}\\modules\\toolkit\\include",
	"%{wks.location}\\dependencies\\fmod", 
	"{wks.location}\\dependencies\\cereal",
	"%{wks.location}\\dependencies\\gzip",
	"%{wks.location}\\dependencies\\tinyGLTF",
	"{wks.location}\\dependencies\\json"
    }

    disablewarnings {
        "26495",
        "26812",
        "26439"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "WINDOWS",
            "FLORP_GLFW"
        }

        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        

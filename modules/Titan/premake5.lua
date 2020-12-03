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
        "tinyGLTF",
		"%{wks.location}\\dependencies\\bullet3\\lib\\Bullet3Common_Debug.lib",
		"%{wks.location}\\dependencies\\bullet3\\lib\\BulletCollision_Debug.lib",
		"%{wks.location}\\dependencies\\bullet3\\lib\\BulletDynamics_Debug.lib",
		"%{wks.location}\\dependencies\\bullet3\\lib\\BulletInverseDynamics_Debug.lib",
		"%{wks.location}\\dependencies\\bullet3\\lib\\BulletSoftBody_Debug.lib",
		"%{wks.location}\\dependencies\\bullet3\\lib\\LinearMath_Debug.lib"
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
	"%{wks.location}\\dependencies\\cereal",
	"%{wks.location}\\dependencies\\gzip",
	"%{wks.location}\\dependencies\\tinyGLTF",
	"%{wks.location}\\dependencies\\json",
	"%{wks.location}\\dependencies\\bullet3\\include",
    }

	DependenciesDebug = {
	"dependencies/bullet3/lib/Bullet3Common_Debug.lib",
	"dependencies/bullet3/lib/BulletCollision_Debug.lib",
	"dependencies/bullet3/lib/BulletDynamics_Debug.lib",
	"dependencies/bullet3/lib/BulletInverseDynamics_Debug.lib",
	"dependencies/bullet3/lib/BulletSoftBody_Debug.lib",
	"dependencies/bullet3/lib/LinearMath_Debug.lib",
}

	DependenciesRelease = {
	"dependencies/bullet3/lib/Bullet3Common.lib",
	"dependencies/bullet3/lib/BulletCollision.lib",
	"dependencies/bullet3/lib/BulletDynamics.lib",
	"dependencies/bullet3/lib/BulletInverseDynamics.lib",
	"dependencies/bullet3/lib/BulletSoftBody.lib",
	"dependencies/bullet3/lib/LinearMath.lib",
}

    disablewarnings {
        "26495",
        "26812",
        "26439"
    }

	postbuildcommands {
    "(xcopy /Q /E /Y /I /C \"%{prj.location}res\" \"%{wks.location}shared_assets\\res\")"
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
        
solution "gmsv_mongolink"

	language "C++"
	location ( "projects/" .. os.get() )
	flags { "Symbols", "NoEditAndContinue", "NoPCH", "EnableSSE" }
	targetdir ( "builds/" .. os.get() .. "/" )
	includedirs { "include/" }

	targetname ("gmsv_mongolink")
	if (os.is("windows")) then
		targetsuffix ("_win32")
		libdirs { "lib/", "C:/local/boost_1_55_0/lib32-msvc-10.0" }
		includedirs { "C:/local/boost_1_55_0/" }
		links { "mongoclient", "bootil_static" }
	elseif (os.is("linux")) then
		targetsuffix ("_linux")
	end
	--targetextension (".dll")
	
	configurations { "Release" }
	
	configuration "Release"
		defines { "NDEBUG" }
		flags{ "Optimize", "FloatFast" }
	
	project "gmsv_mongolink"
		defines { "GMMODULE", "_CRT_SECURE_NO_WARNINGS" }
		files { "src/**.*" }
		kind "SharedLib"
		
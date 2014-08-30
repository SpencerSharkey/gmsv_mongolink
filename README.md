gmsv_mongolink
==============

### Garry's Mod 13 binary module for interfacing with MongoDB databases. ###

* Provides basic MongoDB interface functionality within Garry's Mod.
* **Windows Only** (at this point! haven't gotten around to building on Linux yet)
* Early development phase - not particularly suggested for production use.

### Why? We have tMySQL and MySQLOO for database options!
MongoDB is being used in newer web-based application installations, and is becoming very popular among programmers in the realm of modern environments such as NodeJS. Why not bring this functionality to Garry's Mod? MySQL is tested and proven, but lacks in ease of use compared to MongoDB (imho).

*(This is my first big C++ project, so don't harm me if you find something disgusting)*


**You can use the binaries located in the `builds` folder, or build your own with the following  instructions.** 
The included binaries should be up to date with the source code unless stated otherwise.

### Building your own binaries  ###

* Install prerequisites
 * **Boost** - Requires 1.55.0 _(32-bit VS2010 binaries)_ obtainable from **[here](http://sourceforge.net/projects/boost/files/boost-binaries/1.55.0/boost_1_55_0-msvc-10.0-32.exe/download)**.
  The premake configuration searches for boost library files at `C:/local/boost_1_55_0` *(the default install directory)*
 * **[Bootil](https://github.com/garrynewman/bootil)** - **Included in repository.** Could compile on your own if you'd like to. By Garry Newman.
 * **[MongoClient](https://github.com/mongodb/mongo-cxx-driver/tree/26compat)** - **Included in repository**. 26compat branch built and included for convenience.

* Run `BuildProjects.bat` to generate a Visual Studio 2010 project file with build configuration automatically setup. 
 * Premake can also be configured to generate projects for different versions of Visual Studio. Be aware you may need different Boost libraries for a Visual Studio install other than 2010... *I think*

* Open the Visual Studio file located in `/projects/windows/` and build the project. 
* Outputs to `/builds/windows/gmsv_mongolink_win32.dll` on success!

### Basic Usage ###
You can check out the source code, of course, to see how the module works. But here's a simple example:

```lua
require("mongolink");

local mongo = mongo.New()

mongo:Connect("candidate.19.mongolayer.com:10269")
mongo:Auth("sfuse-prod", "sfdbuser", "skinner452")

PrintTable(mongo:ListCollections())

mongo:Insert("testInsert", {test = true, day = 5})

PrintTable(mongo:Query("testInsert", {day = 5}))
```


### Contribution guidelines ###

* Please help out if you can! This module has potential to be very useful.

### Author ###

* Spencer Sharkey 
 * (spencer@sf-n.com) or [Steam](http://steamcommunity.com/id/sassharkey)

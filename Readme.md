# Graphics Application
Thanks for trying this customizable graphics application! Before customizing this application, please finish the following steps first to make sure the solution can be executed properly.
### 1. Set up debugging environment to enable in editor debugging
  - Right-click ***ExampleGame*** project and choose **Properties**
  - At top, change **Configuration** to **All Configurations** and change **Platform** to **All Platforms**
  - In the tree view to the left select **Configuration Properties->Debugging**
  - Change **Command** to **$(GameInstallDir)$(TargetFileName)**
  - Change **Working Directory** to **$(GameInstallDir)**

### 2. Link Maya with the solution in order to use the custom Maya exporter plug-in
  - Open ***Control Panel*** and select **System**
  - Select **Advanced system settings** from left
  - Select **Environment Variables...**
  - Under **System variables** select **New...**
  - Add an environment variable with variable name being **MAYA_LOCATION** and variable value being the installation location for Maya, the default installation location for Maya 2018 should be **C:\Program Files\Autodesk\Maya2018**, no trailing slash should be added into the path
  - Add an environment variable with variable name being **MAYA_PLUG_IN_PATH** and variable value being the location you want to use to store Maya plug-ins, one example would be **C:\Users\USERNAME\Documents\maya\2018\plug-ins**, no trailing slash should be added into the path
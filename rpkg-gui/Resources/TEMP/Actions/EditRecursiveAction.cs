using System;
using System.Collections.Generic;
using System.IO;
using rpkg.Extensions;
using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.TEMP.Actions
{
    [ResourceAction("TEMP", "Edit in Brick/Entity Editor (Recursive)")]
    public class EditRecursiveAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            string rpkgFile = Path.GetFileName(rpkgPath);

            string rpkgUpperName = rpkgFile.ToUpper();

            if (rpkgUpperName.Contains("PATCH"))
            {
                string baseFileName = rpkgFile.Substring(0, rpkgUpperName.IndexOf("PATCH")).ToUpper();

                string folderPath = Path.GetDirectoryName(rpkgPath);

                List<string> rpkgFiles = new List<string>();

                foreach (var filePath in Directory.GetFiles(folderPath))
                {
                    if (filePath.ToUpper().EndsWith(".RPKG"))
                    {
                        if (filePath.ToUpper().Contains("\\" + baseFileName.ToUpper() + ".RPKG"))
                        {
                            rpkgFiles.Add(filePath);
                        }
                        else if (filePath.ToUpper().Contains("\\" + baseFileName.ToUpper() + "PATCH"))
                        {
                            rpkgFiles.Add(filePath);
                        }
                    }
                }

                rpkgFiles = rpkgFiles.SortNaturally();

                RpkgManager.LoadRpkgs(rpkgFiles);
            }
            
            //WindowUtils.MessageBoxShow(rpkgFilePath);

            int temp_file_version = RpkgLib.get_temp_version(resourceHash, rpkgPath);

            while (temp_file_version < 2 || temp_file_version > 3)
            {
                MessageQuestion messageBox = new MessageQuestion();

                if (temp_file_version == 4)
                {
                    messageBox.message.Content =
                        "The automatic TEMP/TBLU version check found a TEMP entry count but was still unable to determine what version of Hitman (H2 or H3) these files are.\n\nPlease select the correct version of Hitman H2 or H3 below.";
                }
                else if (temp_file_version == 5)
                {
                    messageBox.message.Content =
                        "The automatic TEMP/TBLU version check could not find a TEMP entry count,\n\nmost likely because this TEMP file was made by a version of ResourceTool that didn't include the TEMP subEntities count value.\n\nTherefore the version of Hitman (H2 or H3) was not able to be determined.\n\nPlease select the correct version of Hitman H2 or H3 below.";
                }
                else
                {
                    messageBox.message.Content =
                        "The automatic TEMP/TBLU version check was unable to determine what version of Hitman (H2 or H3) these files are.\n\nPlease select the correct version of Hitman H2 or H3 below.";
                }

                messageBox.OKButton.Content = "Hitman 2";
                messageBox.CancelButton.Content = "Hitman 3";
                messageBox.ShowDialog();

                if (messageBox.buttonPressed == "OKButton")
                {
                    temp_file_version = 2;
                }
                else if (messageBox.buttonPressed == "CancelButton")
                {
                    temp_file_version = 3;
                }
            }

            RpkgLib.clear_temp_tblu_data();

            //WindowUtils.MessageBoxShow(hashName + ", " + rpkgFilePath);
            
            RpkgLib.reset_task_status();

            RpkgLib.load_recursive_temps(resourceHash, rpkgPath, (uint)temp_file_version);

            Progress temp_progress = new Progress();

            temp_progress.message.Content = "Analyzing Entity/Brick (TEMP/TBLU)...";

            temp_progress.operation = (int)Progress.Operation.TEMP_TBLU;

            temp_progress.ShowDialog();

            int temp_index = RpkgLib.get_temp_index(resourceHash);

            //WindowUtils.MessageBoxShow(temp_index.ToString());

            if (temp_progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
            {
                if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_DEPENDS)
                {
                    WindowUtils.MessageBoxShow("Error: " + resourceHash + " file has no TBLU hash depends.");
                }
                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_RPKG)
                {
                    if (rpkgUpperName.Contains("PATCH"))
                    {
                        string rpkgBaseName = rpkgFile.Substring(0, rpkgUpperName.IndexOf("PATCH")) + ".rpkg";

                        WindowUtils.MessageBoxShow(
                            "Error: TBLU file linked to " + resourceHash +
                            " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.\n\nTry importing " +
                            rpkgBaseName + " and trying to edit " + resourceHash +
                            " again.\n\nThis should be done before trying to launch the Brick/Entity Editor."
                        );
                    }
                    else
                    {
                        WindowUtils.MessageBoxShow(
                            "Error: TBLU file linked to " + resourceHash +
                            " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG."
                        );
                    }
                }
                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_TOO_MANY)
                {
                    WindowUtils.MessageBoxShow("Error: " + resourceHash + " file has too many TBLU hash depends.");
                }
                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_HEADER_NOT_FOUND)
                {
                    WindowUtils.MessageBoxShow(
                        "Error: " + resourceHash + " file is an empty TEMP file, missing it's resource type header/footer."
                    );
                }
                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_ENTRY_COUNT_MISMATCH)
                {
                    WindowUtils.MessageBoxShow(
                        "Error: " + resourceHash + " file and TBLU file have mismatched entry/entity counts."
                    );
                }
                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_VERSION_UNKNOWN)
                {
                    WindowUtils.MessageBoxShow("Error: " + resourceHash + " file's version is unknown.");
                }
                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TBLU_VERSION_UNKNOWN)
                {
                    WindowUtils.MessageBoxShow("Error: " + resourceHash + " file's TBLU file's version is unknown.");
                }

                RpkgLib.clear_temp_tblu_data();
            }
            else
            {
                var entityBrickEditor = new EntityBrickEditor();
                

                string initialFolder = "";

                if (File.Exists(App.Settings.InputFolder))
                {
                    initialFolder = App.Settings.InputFolder;
                }
                else
                {
                    initialFolder = Directory.GetCurrentDirectory();
                }

                entityBrickEditor.inputFolder = initialFolder;


                if (File.Exists(App.Settings.OutputFolder))
                {
                    initialFolder = App.Settings.OutputFolder;
                }
                else
                {
                    initialFolder = Directory.GetCurrentDirectory();
                }

                entityBrickEditor.outputFolder = initialFolder;

                entityBrickEditor.temps_index = (uint)temp_index;
                entityBrickEditor.temp_file_version = temp_file_version;
                entityBrickEditor.tempFileName = resourceHash;
                entityBrickEditor.rpkgFilePath = rpkgPath;
                entityBrickEditor.tempFileNameFull = resourceHash; // TODO (OrfeasZ)

                string[] theme = App.Settings.ColorTheme.Split('/');

                entityBrickEditor.currentThemeBrightness = theme[0];
                string color = theme[1];

                entityBrickEditor.ShowDialog();

                // TODO (OrfeasZ)
                /*SetDiscordStatus("Brick Editor", hashName);


                if (LeftTabControl.SelectedIndex == 0)
                {
                    SetDiscordStatus("Resource View", "");
                }
                else if (LeftTabControl.SelectedIndex == 1)
                {
                    SetDiscordStatus("Dependency View", "");
                }
                else if (LeftTabControl.SelectedIndex == 2)
                {
                    SetDiscordStatus("Search View", "");
                }*/

                //GC.Collect();
                GC.WaitForPendingFinalizers();
                //GC.Collect();
            }
        }

        public void Dispose()
        {
        }
    }
}

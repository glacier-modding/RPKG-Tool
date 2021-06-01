using System.IO;
using System.Text.Json;
using Ookii.Dialogs.Wpf;

namespace rpkg.Utils
{
    static class WindowUtils
    {
        public static void MessageBoxShow(string messageBoxString)
        {
            var messageBox = new Message();

            messageBox.message.Content = messageBoxString;

            messageBox.ShowDialog();
        }

        public static string SelectFolder(string type, string description)
        {
            var folderDialog = new VistaFolderBrowserDialog
            {
                Description = description, 
                UseDescriptionForTitle = true
            };

            if (type == "input")
            {
                string initialFolder = "";

                if (File.Exists(App.Settings.InputFolder))
                {
                    initialFolder = App.Settings.InputFolder;
                }
                else
                {
                    initialFolder = Directory.GetCurrentDirectory();
                }

                folderDialog.SelectedPath = initialFolder;
            }
            else if (type == "output")
            {
                string initialFolder = "";

                if (File.Exists(App.Settings.OutputFolder))
                {
                    initialFolder = App.Settings.OutputFolder;
                }
                else
                {
                    initialFolder = Directory.GetCurrentDirectory();
                }

                folderDialog.SelectedPath = initialFolder;
            }

            var folderDialogResult = folderDialog.ShowDialog();

            if (folderDialogResult == true)
            {
                if (type == "input")
                {
                    App.Settings.InputFolder = folderDialog.SelectedPath;
                }
                else if (type == "output")
                {
                    App.Settings.OutputFolder = folderDialog.SelectedPath;
                }

                var options = new JsonSerializerOptions { WriteIndented = true };

                string jsonString = JsonSerializer.Serialize(App.Settings, options);

                File.WriteAllText("rpkg.json", jsonString);

                return folderDialog.SelectedPath;
            }

            return "";
        }

    }
}

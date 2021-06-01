using System;
using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.TEXT.Actions
{
    [BulkResourceAction("TEXT", "Convert All To TGA")]
    public class BulkConvertTgaAction : IBulkAction
    {
        public void PerformBulk(string rpkgPath, string resourceType)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceType + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.MASS_EXTRACT;

            progress.message.Content = "Extracting All " + resourceType + " To TGA...";

            MainWindow.ExtractTEXT("-extract_all_text_from", rpkgPath, resourceType, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}

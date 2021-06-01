using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.WWES.Actions
{
    [BulkResourceAction("WWES", "Convert All To OGG To IOI Paths")]
    public class BulkConvertOggAction : IBulkAction
    {
        public void PerformBulk(string rpkgPath, string resourceType)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceType + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.GENERAL;

            progress.message.Content = "Extracting All " + resourceType + " To OGG To IOI Paths...";

            RpkgLib.task_execute("-extract_wwes_to_ogg_from", rpkgPath, resourceType, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}

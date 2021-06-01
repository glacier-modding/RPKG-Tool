using rpkg.ResourceActions;
using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.Resources.PRIM.Actions
{
    [BulkResourceAction("PRIM", "Convert All to GLB")]
    public class BulkConvertGlbAction : IBulkAction
    {
        public void PerformBulk(string rpkgPath, string resourceType)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceType + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();

            var progress = new Progress();

            progress.operation = (int)Progress.Operation.MASS_EXTRACT;

            progress.message.Content = "Extracting All " + resourceType + " To GBL...";

            RpkgLib.task_execute("-extract_all_prim_to_glb_from", rpkgPath, resourceType, "", "", outputFolder);

            progress.ShowDialog();
        }

        public void Dispose()
        {
        }
    }
}

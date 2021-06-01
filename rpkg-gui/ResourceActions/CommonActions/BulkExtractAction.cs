using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.ResourceActions.CommonActions
{
    [BulkCommonAction("Extract All")]
    public class BulkExtractAction : IBulkAction
    {
        public void PerformBulk(string rpkgPath, string resourceType)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceType + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();
            RpkgLib.task_execute("-extract_from_rpkg", rpkgPath, resourceType, "", "", outputFolder);
        }

        public void Dispose()
        {
        }
    }
}

using rpkg.ResourceActions.Attributes;
using rpkg.Utils;

namespace rpkg.ResourceActions.CommonActions
{
    [CommonAction("Extract")]
    public class ExtractAction : IAction
    {
        public void Perform(string rpkgPath, string resourceHash)
        {
            var outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + resourceHash + " To:");

            if (outputFolder == "")
                return;

            RpkgLib.reset_task_status();
            RpkgLib.task_execute("-extract_from_rpkg", rpkgPath, resourceHash, "", "", outputFolder);
        }

        public void Dispose()
        {
        }
    }
}

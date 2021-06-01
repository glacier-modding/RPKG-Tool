using System.IO;

namespace rpkg
{
    public class UserSettings
    {
        public string InputFolder { get; set; } = Directory.GetCurrentDirectory();
        public string OutputFolder { get; set; } = Directory.GetCurrentDirectory();
        public string ColorTheme { get; set; } = "Dark/Red";
    }
}

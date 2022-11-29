using System.Windows;
using MahApps.Metro.Controls;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MapImportSelector.xaml
    /// </summary>
    public partial class MapImportSelector : MetroWindow
    {
        public MapImportSelector()
        {
            InitializeComponent();
        }

        private void EntityJSONButton_Click(object sender, RoutedEventArgs e)
        {
            qn_format = "entity";

            Close();
        }

        private void PatchJSONButton_Click(object sender, RoutedEventArgs e)
        {
            qn_format = "patch";

            Close();
        }

        public string qn_format = "entity";
    }
}

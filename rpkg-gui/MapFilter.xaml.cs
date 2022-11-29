using System.Windows;
using MahApps.Metro.Controls;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MapFilter.xaml
    /// </summary>
    public partial class MapFilter : MetroWindow
    {
        public MapFilter()
        {
            InitializeComponent();
        }

        private void SubmitButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using MahApps.Metro.Controls;
using ControlzEx.Theming;

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

            this.Close();
        }

        private void PatchJSONButton_Click(object sender, RoutedEventArgs e)
        {
            qn_format = "patch";

            this.Close();
        }

        public string qn_format = "entity";
    }
}

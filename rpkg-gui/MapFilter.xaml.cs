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
            this.Close();
        }
    }
}

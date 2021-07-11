using MahApps.Metro.Controls;
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

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MapEditorViewport.xaml
    /// </summary>
    public partial class MapEditorViewport : Window
    {
        private bool foo;

        public MapEditorViewport()
        {
            InitializeComponent();
            foo = true;
        }

        private void testBtn_Click(object sender, RoutedEventArgs e)
        {
            if (foo)
            {
                try
                {
                    testViewPort.Child = new G3dHwndHost(testViewPort.Width, testViewPort.Height);
                    foo = false;
                }
                catch (Exception) { }
            }
        }
    }
}

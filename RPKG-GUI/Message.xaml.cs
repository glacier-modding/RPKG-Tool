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
    /// Interaction logic for Message.xaml
    /// </summary>
    public partial class Message : MetroWindow
    {
        public Message()
        {
            InitializeComponent();
        }
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);

            if (sizeInfo.HeightChanged)
                this.Top += (sizeInfo.PreviousSize.Height - sizeInfo.NewSize.Height) / 2;

            if (sizeInfo.WidthChanged)
                this.Left += (sizeInfo.PreviousSize.Width - sizeInfo.NewSize.Width) / 2;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void OKButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}

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
    /// Interaction logic for Filter.xaml
    /// </summary>
    public partial class Filter : MetroWindow
    {
        public Filter()
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

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            filterString = FilterInput.Text;

            this.Close();
        }

        public string filterString = "";
    }
}

using System.Windows;
using System.Windows.Controls;
using MahApps.Metro.Controls;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MessageQuestion.xaml
    /// </summary>
    public partial class MessageQuestion : MetroWindow
    {
        public MessageQuestion()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

        }
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);

            if (sizeInfo.HeightChanged)
                Top += (sizeInfo.PreviousSize.Height - sizeInfo.NewSize.Height) / 2;

            if (sizeInfo.WidthChanged)
                Left += (sizeInfo.PreviousSize.Width - sizeInfo.NewSize.Width) / 2;
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            buttonPressed = (sender as Button).Name;

            Close();
        }

        private void OKButton_Click(object sender, RoutedEventArgs e)
        {
            buttonPressed = (sender as Button).Name;

            Close();
        }

        public string buttonPressed = "";
    }
}

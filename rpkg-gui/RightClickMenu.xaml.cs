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
    /// Interaction logic for RightClickMenu.xaml
    /// </summary>
    public partial class RightClickMenu : MetroWindow
    {
        public RightClickMenu(string[] buttons)
        {
            InitializeComponent();

            for (int i = 0; i < buttons.Length; i++)
            {
                System.Windows.Controls.Button newButton = new Button();

                newButton.Content = buttons[i];
                newButton.Name = "button" + i.ToString();
                newButton.Click += this.Button_Click;
                newButton.Height = 34;
                newButton.Padding = new Thickness(8, 0, 8, 0);

                if (buttons.Length == 1)
                {
                    newButton.Margin = new Thickness(8, 8, 8, 8);
                }
                else if (i == 0)
                {
                    newButton.Margin = new Thickness(8, 8, 8, 4);
                }
                else if (i == (buttons.Length - 1))
                {
                    newButton.Margin = new Thickness(8, 4, 8, 8);
                }
                else
                {
                    newButton.Margin = new Thickness(8, 4, 8, 4);
                }

                this.MainStackPanel.Children.Add(newButton);
            }
        }

        public void Button_Click(object sender, RoutedEventArgs e)
        {
            buttonPressed = (sender as Button).Name;

            this.Close();
        }

        public string buttonPressed = "";
    }
}

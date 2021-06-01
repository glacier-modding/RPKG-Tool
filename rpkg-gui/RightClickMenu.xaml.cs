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
        public string buttonPressed = "";
        public int PressedButtonIndex = -1;

        public RightClickMenu(string[] buttons)
        {
            InitializeComponent();

            for (int i = 0; i < buttons.Length; i++)
            {
                System.Windows.Controls.Button newButton = new Button();

                newButton.Content = buttons[i];
                newButton.Name = "button" + i.ToString();
                newButton.Click += this.Button_Click;
                newButton.Height = 40;
                newButton.Padding = new Thickness(16, 0, 16, 0);

                if (buttons.Length == 1)
                {
                    newButton.Margin = new Thickness(16, 16, 16, 16);
                }
                else if (i == 0)
                {
                    newButton.Margin = new Thickness(16, 16, 16, 8);
                }
                else if (i == (buttons.Length - 1))
                {
                    newButton.Margin = new Thickness(16, 8, 16, 16);
                }
                else
                {
                    newButton.Margin = new Thickness(16, 8, 16, 8);
                }
                
                this.MainStackPanel.Children.Add(newButton);
            }
        }

        public void Button_Click(object sender, RoutedEventArgs e)
        {
            buttonPressed = (sender as Button).Name;
            PressedButtonIndex = int.Parse(buttonPressed.Substring(6));

            this.Close();
        }
    }
}

using System;
using System.Windows;
using MahApps.Metro.Controls;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for ColorPicker.xaml
    /// </summary>
    public partial class ColorPicker : MetroWindow
    {
        public ColorPicker()
        {
            InitializeComponent();
        }

        private void MetroWindow_Loaded(object sender, RoutedEventArgs e)
        {
            ColorCanvas.R = FloatToByte(red);
            ColorCanvas.G = FloatToByte(green);
            ColorCanvas.B = FloatToByte(blue);
        }

        private byte FloatToByte(float input)
        {
            float temp_float = 255.0F * input;

            int temp_int = (int)temp_float;

            if (temp_int > 255)
            {
                temp_int = 255;
            }
            else if (temp_int < 0)
            {
                temp_int = 0;
            }

            byte[] value = BitConverter.GetBytes(temp_int);

            return value[0];
        }

        public float red = 0;
        public float green = 0;
        public float blue = 0;
    }
}

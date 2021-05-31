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
using System.IO;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Threading;
using MahApps.Metro.Controls;
using ControlzEx.Theming;
using System.Security.Cryptography;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for HashCalculator.xaml
    /// </summary>
    public partial class HashCalculator : MetroWindow
    {
        public HashCalculator()
        {
            InitializeComponent();
        }

        private void IOIHashCalculator_Loaded(object sender, RoutedEventArgs e)
        {
            InputTextBox.Focus();
        }

        private void InputTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (hashInputTimer == null)
            {
                hashInputTimer = new System.Windows.Threading.DispatcherTimer();

                hashInputTimer.Interval = TimeSpan.FromMilliseconds(600);

                hashInputTimer.Tick += SearchTEMPsTextBox_TimerTimeout;
            }

            hashInputTimer.Stop();
            hashInputTimer.Start();
        }

        private void SearchTEMPsTextBox_TimerTimeout(object sender, EventArgs e)
        {
            var timer = (sender as System.Windows.Threading.DispatcherTimer);

            if (timer == null)
            {
                return;
            }

            timer.Stop();

            OutputTextBox.Clear();

            foreach (string line in InputTextBox.Text.Split('\n'))
            {
                string lineString = line.Replace("\r", "").Replace("\n", "").ToLower();

                if (lineString != "")
                {
                    var md5 = new MD5CryptoServiceProvider();

                    byte[] stringBytes = Encoding.UTF8.GetBytes(lineString);

                    var buffer = md5.ComputeHash(stringBytes);

                    var sb = new StringBuilder();

                    for (var i = 1; i < 8; i++)
                    {
                        sb.Append(buffer[i].ToString("X2").PadLeft(2, '0'));
                    }

                    string ioiHash = "00" + sb.ToString();

                    int return_value = search_hash_list_by_hash_value(ioiHash);

                    if (return_value == 1)
                    {
                        OutputTextBox.Text += ioiHash + " - Found in hash list" + "\n";
                    }
                    else
                    {
                        OutputTextBox.Text += ioiHash + " - Not found in hash list" + "\n";
                    }
                }
            }
        }


        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private System.Windows.Threading.DispatcherTimer hashInputTimer;

        [DllImport("rpkg.dll", EntryPoint = "search_hash_list_by_hash_value", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_hash_list_by_hash_value(string hash_value);
    }
}

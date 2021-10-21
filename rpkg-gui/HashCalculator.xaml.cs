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
using System.Buffers;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for HashCalculator.xaml
    /// </summary>
    public partial class HashCalculator : MetroWindow
    {
        private class Canceller
        {
            // I tried using CancellationTokenSource, but that was giving me weird slowdowns
            // -grappigegovert
            public bool shouldCancel = false;
        }

        private HashSet<String> newlyFoundHashes = new HashSet<string>();
        private Canceller hashCalculationCanceller;

        public HashCalculator()
        {
            InitializeComponent();
        }

        private void IOIHashCalculator_Loaded(object sender, RoutedEventArgs e)
        {
            InputTextBox.Focus();
        }

        private async void InputTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (hashCalculationCanceller != null)
            {
                // cancel the ongoing calculation
                hashCalculationCanceller.shouldCancel = true;
            }
            Canceller canceller = new Canceller();
            hashCalculationCanceller = canceller;

            string input = InputTextBox.Text;
            CalculatingLabel.Visibility = Visibility.Visible;
            string output = await Task.Run(() => checkHashes(input, canceller));
            if (output != null)
            {
                OutputTextBox.Text = output;
                CalculatingLabel.Visibility = Visibility.Hidden;
            }
        }

        private string checkHashes(string inputtext, Canceller canceller)
        {
            MD5CryptoServiceProvider md5 = new MD5CryptoServiceProvider();

            if (canceller.shouldCancel)
            {
                return null;
            }

            string[] lines = inputtext.Split('\n', '\r');

            StringBuilder outputTextBuilder = new StringBuilder(lines.Length * 37);

            foreach (string line in lines)
            {
                if (canceller.shouldCancel)
                {
                    return null;
                }

                string lineString = line.Trim().ToLower();

                if (lineString != "")
                {
                    byte[] stringBytes = Encoding.UTF8.GetBytes(lineString);
                    byte[] hashBytes = md5.ComputeHash(stringBytes);

                    StringBuilder ioiHashsb = new StringBuilder("00", 16);
                    for (int i = 1; i < 8; i++)
                    {
                        ioiHashsb.Append(hashBytes[i].ToString("X2"));
                    }
                    string ioiHash = ioiHashsb.ToString();

                    // 16 hex chars + 1 period + 4 extension chars + 1 null-ternimator = 22 chars
                    char[] charbuffer = ArrayPool<char>.Shared.Rent(22);
                    bool found = false;
                    string hash_name = "";
                    try
                    {
                        found = get_hash_name_from_hash_value(Convert.ToUInt64(ioiHash, 16), charbuffer);
                        hash_name = new string(charbuffer, 0, 21);
                    }
                    finally
                    {
                        ArrayPool<char>.Shared.Return(charbuffer);
                    }

                    if (found)
                    {
                        newlyFoundHashes.Add($"{hash_name.ToUpper()},{lineString}");
                        outputTextBuilder.AppendLine($"{ioiHash} - Found in hash list");
                    }
                    else
                    {
                        outputTextBuilder.AppendLine($"{ioiHash} - Not found in hash list");
                    }
                }
            }
            return outputTextBuilder.ToString();
        }


        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Save_Click(object sender, RoutedEventArgs e)
        {

            string foundHashes = "";

            foreach (string foundHash in newlyFoundHashes)
            {
                foundHashes += (foundHash) + "\n";
            }

            Clipboard.SetText(foundHashes);
        }




        [DllImport("rpkg.dll", EntryPoint = "get_hash_name_from_hash_value", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool get_hash_name_from_hash_value(UInt64 hash_value, [Out, MarshalAs(UnmanagedType.LPArray, SizeConst = 22)] char[] hash_name);

    }

}

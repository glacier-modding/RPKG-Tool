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
using System.Net.Http;
using System.Net;
using SharpCompress.Archives;
using SharpCompress.IO;
using System.Threading;
using System.ComponentModel;
using MahApps.Metro.Controls;
using ControlzEx.Theming;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for DownloadExtractionProgress.xaml
    /// </summary>
    public partial class DownloadExtractionProgress : MetroWindow
    {
        public DownloadExtractionProgress()
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
            if (operation == 0)
            {
                WebClient webClient = new WebClient();

                webClient.DownloadProgressChanged += WebClient_DownloadProgressChanged;

                webClient.DownloadFileCompleted += WebClient_DownloadFileCompleted;

                Uri uri = new Uri("https://hitmandb.glaciermodding.org/latest-hashes.7z");

                webClient.DownloadFileAsync(uri, "latest-hashes.7z");
            }
            else
            {
                BackgroundWorker worker = new BackgroundWorker();
                worker.WorkerReportsProgress = true;
                worker.DoWork += worker_DoWork;
                worker.RunWorkerCompleted += worker_RunWorkerCompleted;

                worker.RunWorkerAsync();
            }            
        }

        private void WebClient_DownloadFileCompleted(object sender, AsyncCompletedEventArgs e)
        {
            this.Close();
        }

        private void WebClient_DownloadProgressChanged(object sender, DownloadProgressChangedEventArgs e)
        {
            ProgressBar.Value = e.ProgressPercentage;
        }

        void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            if (operation == 1)
            {
                using (var archive2 = ArchiveFactory.Open("latest-hashes.7z"))
                {
                    foreach (var entry in archive2.Entries)
                    {
                        if (!entry.IsDirectory)
                        {
                            SharpCompress.Common.ExtractionOptions options = new SharpCompress.Common.ExtractionOptions();

                            options.ExtractFullPath = true;
                            options.Overwrite = true;

                            entry.WriteToDirectory(System.IO.Directory.GetCurrentDirectory(), options);
                        }
                    }
                }
            }
            else if (operation == 2)
            {
                HttpClient http = new HttpClient() { Timeout = TimeSpan.FromSeconds(10) };

                int.TryParse(http.GetAsync("https://hitmandb.glaciermodding.org/version").Result.Content.ReadAsStringAsync().Result, out currentVersionAvailable);
            }
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.Close();
        }

        public int operation = 0;
        public int currentVersionAvailable = 0;
    }
}

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using ControlzEx.Theming;
using DiscordRPC;
using HelixToolkit.Wpf;
using MahApps.Metro.Controls;
using NAudio.Wave;
using Ookii.Dialogs.Wpf;
using rpkg.Extensions;
using rpkg.ResourceActions;
using rpkg.Utils;
using Application = System.Windows.Application;
using Button = System.Windows.Controls.Button;
using MenuItem = System.Windows.Controls.MenuItem;
using Point = System.Windows.Point;
using TabControl = System.Windows.Controls.TabControl;
using TextBox = System.Windows.Controls.TextBox;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        public DiscordRpcClient Client { get; private set; }

        private void DiscordRPCSetup()
        {
            Client = new DiscordRpcClient("845255830110732290");
            Client.Initialize();
        }

        public void SetDiscordStatus(string details, string state)
        {
            if (discordOn)
            {
                Client.SetPresence(new RichPresence
                {
                    Details = details,
                    State = state,
                    Assets = new Assets
                    {
                        LargeImageKey = "icon",
                        LargeImageText = "RPKG Tool"
                    },
                    Timestamps = timestamp,
                    Buttons = new[]
                    {
                        new DiscordRPC.Button { Label = "Download RPKG Tool", Url = "https://notex.app" }
                    }
                });
            }
        }

        private void DiscordRichPresence_Toggled(object sender, RoutedEventArgs e)
        {
            if ((sender as ToggleSwitch).IsOn)
            {
                if (!discordOn)
                {
                    discordOn = true;

                    DiscordRPCSetup();

                    SetDiscordStatus("Idle", "");

                    if (oneOrMoreRPKGsHaveBeenImported)
                    {
                        if (LeftTabControl.SelectedIndex == 0)
                        {
                            SetDiscordStatus("Resource View", "");
                        }
                        else if (LeftTabControl.SelectedIndex == 1)
                        {
                            SetDiscordStatus("Dependency View", "");
                        }
                        else if (LeftTabControl.SelectedIndex == 2)
                        {
                            SetDiscordStatus("Search View", "");
                        }
                    }
                }
            }
            else
            {
                if (discordOn)
                {
                    discordOn = false;

                    Client.Dispose();
                }
            }
        }
        private void DiscordInvite_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Process.Start("https://discord.com/invite/hxPT9rf");
        }

        public MainWindow()
        {
            InitializeComponent();

            ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.DodgerBlue)));

            timestamp = Timestamps.Now;

            RpkgManager.RpkgLoaded += OnRpkgLoaded;
            RpkgManager.RpkgUnloaded += OnRpkgUnloaded;
            RpkgManager.RpkgReloaded += OnRpkgReloaded;
        }

        private void OnRpkgLoaded(string rpkgPath)
        {
            if (MainTreeView.Nodes.Count > 0)
            {
                if (MainTreeView.Nodes[0].Text == "Click")
                {
                    MainTreeView.Nodes.Clear();
                }
            }

            MainTreeView.AfterExpand += MainTreeView_AfterExpand;

            var item = new TreeNode();

            item.Text = rpkgPath;

            MainTreeView.Nodes.Add(item);

            List<string> resourceTypes = new List<string>();

            uint resourceTypeCount = RpkgLib.get_resource_types_count(rpkgPath);

            for (uint i = 0; i < resourceTypeCount; i++)
            {
                resourceTypes.Add(Marshal.PtrToStringAnsi(RpkgLib.get_resource_types_at(rpkgPath, i)));
            }

            resourceTypes.Sort();

            foreach (string resourceType in resourceTypes)
            {
                var item2 = new TreeNode();

                item2.Text = resourceType;

                item2.Nodes.Add("");

                //item2.Collapsed += Item2_Collapsed;

                item.Nodes.Add(item2);
            }

            if (oneOrMoreRPKGsHaveBeenImported)
            {
                if (LeftTabControl.SelectedIndex == 0)
                {
                    SetDiscordStatus("Resource View", "");
                }
                else if (LeftTabControl.SelectedIndex == 1)
                {
                    SetDiscordStatus("Dependency View", "");
                }
                else if (LeftTabControl.SelectedIndex == 2)
                {
                    SetDiscordStatus("Search View", "");
                }
            }

            oneOrMoreRPKGsHaveBeenImported = true;
        }
        
        private void OnRpkgUnloaded(string rpkgPath)
        {
            SearchRPKGsTreeView.Nodes.Clear();

            var treeviewItemFound = false;
            var treeviewItemIndex = 0;

            for (var i = 0; i < MainTreeView.Nodes.Count; i++)
            {
                var item = MainTreeView.Nodes[i];

                if (item.Text != rpkgFilePath)
                    continue;

                treeviewItemIndex = i;
                treeviewItemFound = true;

                break;
            }

            if (treeviewItemFound)
                MainTreeView.Nodes.RemoveAt(treeviewItemIndex);
        }

        private void OnRpkgReloaded(string rpkgPath)
        {
            SearchRPKGsTreeView.Nodes.Clear();

            foreach (TreeNode item in MainTreeView.Nodes)
            {
                if (item.Text == rpkgFilePath)
                {
                    MainTreeView.SelectedNode = item;
                }
            }

            var treeviewItemFound = false;
            var count = 0;
            var treeviewItemIndex = 0;

            foreach (TreeNode item in HashMapTreeView.Nodes)
            {
                if (item.Text == rpkgFilePath)
                {
                    treeviewItemIndex = count;

                    treeviewItemFound = true;
                }

                count++;
            }

            if (treeviewItemFound)
            {
                HashMapTreeView.Nodes.RemoveAt(treeviewItemIndex);
            }
        }

        private void DownloadExtractHashList()
        {
            DownloadExtractionProgress downloadExtractionProgress1 = new DownloadExtractionProgress();
            downloadExtractionProgress1.operation = 0;
            downloadExtractionProgress1.message.Content = "Downloading https://hitmandb.notex.app/latest-hashes.7z...";
            downloadExtractionProgress1.ShowDialog();

            DownloadExtractionProgress downloadExtractionProgress2 = new DownloadExtractionProgress();
            downloadExtractionProgress2.operation = 1;
            downloadExtractionProgress2.ProgressBar.IsIndeterminate = true;
            downloadExtractionProgress2.message.Content = "Extracting hash__list.txt from latest-hashes.7z...";
            downloadExtractionProgress2.ShowDialog();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            AddHandlers();

            if (!File.Exists("rpkg.json"))
            {
                SetColorTheme("Dark", "Red");

                var options = new JsonSerializerOptions {WriteIndented = true};

                string jsonString = JsonSerializer.Serialize(App.Settings, options);

                File.WriteAllText("rpkg.json", jsonString);
            }
            else
            {
                string jsonString = File.ReadAllText("rpkg.json");

                App.Settings = JsonSerializer.Deserialize<UserSettings>(jsonString);

                string[] theme = App.Settings.ColorTheme.Split('/');

                string brightness = theme[0];
                string color = theme[1];

                SetColorTheme(brightness, color);
            }

            if (!File.Exists("hash_list.txt"))
            {
                MessageQuestion messageBox = new MessageQuestion();
                messageBox.message.Content = "Error: The hast list file (hash_list.txt) is missing.\n\nIt is necessary to have this file for the optimal functioning of this program.\n\nClick OK to have it downloaded and extracted manually.\n\nThe program will automatically restart when done.\n\nYou can also download it manually from https://hitmandb.notex.app/latest-hashes.7z and extract it to the same directory as this program.";
                messageBox.ShowDialog();

                //WindowUtils.MessageBoxShow(messageBox.buttonPressed);

                if (messageBox.buttonPressed == "OKButton")
                {
                    DownloadExtractHashList();
                    System.Windows.Forms.Application.Restart();

                    if (discordOn)
                    {
                        Client.Dispose();
                    }

                    Environment.Exit(0);
                }
                else if (messageBox.buttonPressed == "CancelButton")
                {
                    if (discordOn)
                    {
                        Client.Dispose();
                    }

                    Environment.Exit(0);
                }
            }

            int currentVersion = RpkgLib.load_hash_list();

            DownloadExtractionProgress downloadExtractionProgress = new DownloadExtractionProgress();
            downloadExtractionProgress.operation = (int)Progress.Operation.MASS_EXTRACT;
            downloadExtractionProgress.ProgressBar.IsIndeterminate = true;
            downloadExtractionProgress.message.Content = "Checking https://hitmandb.notex.app/version to see if a new hash list is available...";
            downloadExtractionProgress.ShowDialog();

            if (currentVersion < downloadExtractionProgress.currentVersionAvailable)
            {
                MessageQuestion messageBox = new MessageQuestion();
                messageBox.message.Content = "There is a new version of the hash list available.\n\nWould you like to download it now and update your current hash list?\n\nClick OK to have it downloaded and extracted manually.\n\nThe program will automatically restart when done.";
                messageBox.ShowDialog();

                if (messageBox.buttonPressed == "OKButton")
                {
                    DownloadExtractHashList();
                    System.Windows.Forms.Application.Restart();

                    if (discordOn)
                    {
                        Client.Dispose();
                    }

                    Environment.Exit(0);
                }
            }

            var item = new System.Windows.Forms.TreeNode();
            item.Text = "Click";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "File->Import RPKG File";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Or";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "File->Import RPKGs Folder";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Above to start!";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "After any RPKGs have been imported you can left or right click on the RPKG";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "or on any item in the Resources or Dependency Views.";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Left clicking causes the Details/Hex Viewer/JSON Viewer on the right to update.";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Right clicking provides you with a popup menu for further options.";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Alternatively you can select from the:";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Extract";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Generate";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Mass Extract";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Rebuild";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "Color Theme";
            MainTreeView.Nodes.Add(item);
            item = new System.Windows.Forms.TreeNode();
            item.Text = "menus above for additional functions.";
            MainTreeView.Nodes.Add(item);

            MainTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
            HashMapTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
            SearchRPKGsTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
            MainTreeView.AfterSelect += MainTreeView_AfterSelect;
            HashMapTreeView.AfterSelect += MainTreeView_AfterSelect;
            SearchRPKGsTreeView.AfterSelect += MainTreeView_AfterSelect;
            SearchHashListTreeView.AfterSelect += SearchHashListTreeView_AfterSelect;
            LeftTabControl.SelectionChanged += LeftTabControl_SelectionChanged;
            RightTabControl.SelectionChanged += RightTabControl_SelectionChanged;
        }

        private void OnResourceTypeSelected(string resourceType)
        {
            if (LeftTabControl.SelectedIndex == 0)
            {
                SetDiscordStatus("Resource View", resourceType);
            }
            else if (LeftTabControl.SelectedIndex == 1)
            {
                SetDiscordStatus("Dependency View", resourceType);
            }
            else if (LeftTabControl.SelectedIndex == 2)
            {
                SetDiscordStatus("Search View", resourceType);
            }

            HexViewerTextBox.Text = "";

            TabJsonViewer.Visibility = Visibility.Collapsed;
            TabImageViewer.Visibility = Visibility.Collapsed;
            TabAudioPlayer.Visibility = Visibility.Collapsed;
            Tab3dViewer.Visibility = Visibility.Collapsed;

            ItemDetails.ShowResourceTypeDetails(rpkgFilePath, resourceType);
        }

        private void OnRpkgSelected(string rpkgPath)
        {
            string rpkgFile = rpkgPath.Substring(rpkgPath.LastIndexOf("\\") + 1);

            if (LeftTabControl.SelectedIndex == 0)
            {
                SetDiscordStatus("Resource View", rpkgFile);
            }
            else if (LeftTabControl.SelectedIndex == 1)
            {
                SetDiscordStatus("Dependency View", rpkgFile);
            }
            else if (LeftTabControl.SelectedIndex == 2)
            {
                SetDiscordStatus("Search View", rpkgFile);
            }

            HexViewerTextBox.Text = "";
            
            TabJsonViewer.Visibility = Visibility.Collapsed;
            TabImageViewer.Visibility = Visibility.Collapsed;
            TabAudioPlayer.Visibility = Visibility.Collapsed;
            Tab3dViewer.Visibility = Visibility.Collapsed;

            ItemDetails.ShowRpkgDetails(rpkgFilePath);
        }

        private void OnResourceSelected(string resourcePath)
        {
            string[] header = resourcePath.Split(' ');

            string hashFile = header[0];
            string ioiString = header[1];

            string[] hashDetails = hashFile.Split('.');

            string hash = hashDetails[0];
            string resourceType = hashDetails[1];

            if (LeftTabControl.SelectedIndex == 0)
            {
                SetDiscordStatus("Resource View", hashFile);
            }
            else if (LeftTabControl.SelectedIndex == 1)
            {
                SetDiscordStatus("Dependency View", hashFile);
            }
            else if (LeftTabControl.SelectedIndex == 2)
            {
                SetDiscordStatus("Search View", hashFile);
            }

            ItemDetails.ShowResourceDetails(rpkgFilePath, resourcePath);

            currentHashFileName = hashFile;

            hashDependsRPKGFilePath = rpkgFilePath;
            
            //HexViewerTextBox.Text = "Hex view of " + header[0] + ":\n\n";
            //HexViewerTextBoxTextString = "Hex view of " + header[0] + ":\n\n";

            LocalizationTextBox.Text = "";

            //HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash));
            //HexViewerTextBoxTextString += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash));

            if (RightTabControl.SelectedIndex == 1)
            {
                LoadHexEditor();
            }

            string currentRPKGFilePath = rpkgFilePath;

            currentHash = header[0];

            if (resourceType == "JSON")
            {
                uint json_data_size = RpkgLib.generate_json_string(rpkgFilePath, hash);

                byte[] json_data = new byte[json_data_size];

                Marshal.Copy(RpkgLib.get_json_string(), json_data, 0, (int)json_data_size);

                if (json_data_size > 0)
                {
                    LocalizationTextBox.Text = Encoding.UTF8.GetString(json_data);
                }

                if (json_data_size > 0)
                {
                    if (TabJsonViewer.Visibility == Visibility.Collapsed)
                    {
                        TabJsonViewer.Visibility = Visibility.Visible;
                    }
                }
                else
                {
                    if (TabJsonViewer.Visibility == Visibility.Visible)
                    {
                        TabJsonViewer.Visibility = Visibility.Collapsed;
                    }
                }
            }
            else if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
            {
                uint localization_data_size = RpkgLib.generate_localization_string(rpkgFilePath, hash, resourceType);

                byte[] localization_data = new byte[localization_data_size];

                Marshal.Copy(RpkgLib.get_localization_string(), localization_data, 0, (int)localization_data_size);

                if (localization_data_size > 0)
                {
                    LocalizationTextBox.Text = Encoding.UTF8.GetString(localization_data);
                }

                if (localization_data_size > 0)
                {
                    TabJsonViewer.Visibility = Visibility.Visible;
                }
                else
                {
                    TabJsonViewer.Visibility = Visibility.Collapsed;
                }
            }
            else
            {
                TabJsonViewer.Visibility = Visibility.Collapsed;
                TabImageViewer.Visibility = Visibility.Collapsed;
                TabAudioPlayer.Visibility = Visibility.Collapsed;
                Tab3dViewer.Visibility = Visibility.Collapsed;
            }

            int return_value = RpkgLib.clear_hash_data_vector();

            if (resourceType == "PRIM")
            {
                string command = "-extract_prim_to_obj_from";
                string input_path = rpkgFilePath;
                string filter = hash;
                string search = "";
                string search_type = "";
                string output_path = "";

                return_value = RpkgLib.reset_task_status();

                return_value = RpkgLib.task_execute(command, input_path, filter, search, search_type, output_path);

                string currentDirectory = Directory.GetCurrentDirectory();

                List<string> objFileNames = new List<string>();
                List<int> objFileSizes = new List<int>();

                int fileSizeMax = 0;

                int objIndex = 0;
                int objIndexCount = 0;

                foreach (var filePath in Directory.GetFiles(currentDirectory))
                {
                    if (filePath.ToUpper().Contains(hash) && filePath.EndsWith(".obj"))
                    {
                        objFileNames.Add(filePath);

                        if (filePath.Length > fileSizeMax)
                        {
                            fileSizeMax = filePath.Length;

                            objIndex = objIndexCount;
                        }

                        objIndexCount++;
                    }
                }

                if (objFileNames.Count > 0)
                {
                    if (Tab3dViewer.Visibility == Visibility.Collapsed)
                    {
                        Tab3dViewer.Visibility = Visibility.Visible;
                    }

                    ModelImporter import = new ModelImporter();
                    System.Windows.Media.Media3D.Model3DGroup model1 = import.Load(objFileNames[objIndex]);
                    System.Windows.Media.Media3D.Material mat = MaterialHelper.CreateMaterial(new SolidColorBrush(System.Windows.Media.Color.FromRgb(200, 200, 200)));
                    foreach (System.Windows.Media.Media3D.GeometryModel3D geometryModel in model1.Children)
                    {
                        geometryModel.Material = mat;
                        geometryModel.BackMaterial = mat;
                    }
                    model.Content = model1;
                    helixViewport.Camera.ZoomExtents(helixViewport.Viewport, 1000);

                    foreach (string filePath in objFileNames)
                    {
                        File.Delete(filePath);
                    }
                }
                else
                {
                    if (Tab3dViewer.Visibility == Visibility.Visible)
                    {
                        Tab3dViewer.Visibility = Visibility.Collapsed;
                    }
                }
            }

            if (resourceType == "GFXI")
            {
                BitmapImage bitmapImage = new BitmapImage();

                bitmapImage.BeginInit();

                uint hash_size = RpkgLib.get_hash_in_rpkg_size(rpkgFilePath, hash);

                byte[] hash_data = new byte[hash_size];

                Marshal.Copy(RpkgLib.get_hash_in_rpkg_data(rpkgFilePath, hash), hash_data, 0, (int)hash_size);

                MemoryStream memoryStream = new MemoryStream(hash_data);

                bitmapImage.StreamSource = memoryStream;

                bitmapImage.EndInit();

                ImageViewer.Source = bitmapImage;

                if (hash_size > 0)
                {
                    if (TabImageViewer.Visibility == Visibility.Collapsed)
                    {
                        TabImageViewer.Visibility = Visibility.Visible;
                    }
                }
                else
                {
                    if (TabImageViewer.Visibility == Visibility.Visible)
                    {
                        TabImageViewer.Visibility = Visibility.Collapsed;
                    }
                }

                int return_value_clear = RpkgLib.clear_hash_data_vector();
            }

            if (resourceType == "TEXT")
            {
                BitmapImage bitmapImage = new BitmapImage();

                bitmapImage.BeginInit();

                string currentDirectory = Directory.GetCurrentDirectory();

                string png_file_name = currentDirectory + "\\" + hashFile + ".png";

                //return_value = generate_png_from_text(rpkgFilePath, hash, png_file_name);

                Thread thread = new Thread(() => TEXTToPNGThread(rpkgFilePath, hash, png_file_name));
                thread.SetApartmentState(ApartmentState.MTA);
                thread.Start();
                thread.Join();

                if (File.Exists(png_file_name))
                {
                    MemoryStream memoryStream = new MemoryStream(File.ReadAllBytes(png_file_name));

                    bitmapImage.StreamSource = memoryStream;

                    bitmapImage.EndInit();

                    ImageViewer.Source = bitmapImage;

                    if (TabImageViewer.Visibility == Visibility.Collapsed)
                    {
                        TabImageViewer.Visibility = Visibility.Visible;
                    }

                    File.Delete(png_file_name);
                }
                else
                {
                    if (TabImageViewer.Visibility == Visibility.Visible)
                    {
                        TabImageViewer.Visibility = Visibility.Collapsed;
                    }
                }

                int return_value_clear = RpkgLib.clear_hash_data_vector();
            }

            if (resourceType == "WWEM" || resourceType == "WWES")
            {
                if (OGGPlayerTimer != null)
                {
                    OGGPlayerTimer.Stop();
                }

                if (waveOut != null)
                {
                    waveOut.Stop();
                    waveOut.Dispose();
                    waveOut = null;
                }

                oggPlayerState = (int)OggPlayerState.NULL;

                oggPlayerPaused = false;

                OGGPlayerLabelHashFileName.Content = hashFile;
                OGGPlayerLabelIOIString.Content = ioiString;

                OGGPlayer.Value = 0;

                OGGPlayerLabel.Content = "0 / 0";

                OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline;

                if (OGGPlayerComboBox.Visibility == Visibility.Visible)
                {
                    OGGPlayerComboBox.Visibility = Visibility.Collapsed;
                }

                if (TabAudioPlayer.Visibility == Visibility.Collapsed)
                {
                    TabAudioPlayer.Visibility = Visibility.Visible;
                }

                return_value = RpkgLib.create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hash, 0, 0);

                string currentDirectory = Directory.GetCurrentDirectory();

                string inputOGGFile = currentDirectory + "\\" + hash + ".ogg";

                string outputPCMFile = currentDirectory + "\\" + hash + ".pcm";

                return_value = RpkgLib.convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

                if (File.Exists(outputPCMFile))
                {
                    if (pcmMemoryStream != null)
                    {
                        pcmMemoryStream.Dispose();
                    }

                    pcmMemoryStream = new MemoryStream(File.ReadAllBytes(outputPCMFile));

                    int pcmSampleSize = RpkgLib.get_pcm_sample_size();
                    int pcmSampleRate = RpkgLib.get_pcm_sample_rate();
                    int pcmChannels = RpkgLib.get_pcm_channels();

                    File.Delete(hash + ".ogg");
                    File.Delete(hash + ".wem");
                    File.Delete(hash + ".pcm");

                    //file = new FileInfo("output.pcm");
                    //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                    waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                    pcmSource = new RawSourceWaveStream(pcmMemoryStream, waveFormat);

                    oggPlayerState = (int)OggPlayerState.READY;
                }
            }

            if (resourceType == "WWEV")
            {
                if (OGGPlayerTimer != null)
                {
                    OGGPlayerTimer.Stop();
                }

                if (waveOut != null)
                {
                    waveOut.Stop();
                    waveOut.Dispose();
                    waveOut = null;
                }

                oggPlayerPaused = false;

                OGGPlayerLabelHashFileName.Content = hashFile;
                OGGPlayerLabelIOIString.Content = ioiString;

                OGGPlayer.Value = 0;

                OGGPlayerLabel.Content = "0 / 0";

                OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline;

                OGGPlayerComboBox.Items.Clear();

                if (OGGPlayerComboBox.Visibility == Visibility.Collapsed)
                {
                    OGGPlayerComboBox.Visibility = Visibility.Visible;
                }

                int wwevCount = RpkgLib.create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hash, 0, 0);

                if (wwevCount > 0)
                {
                    if (TabAudioPlayer.Visibility == Visibility.Collapsed)
                    {
                        TabAudioPlayer.Visibility = Visibility.Visible;
                    }

                    return_value = RpkgLib.create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hash, 1, 0);

                    string currentDirectory = Directory.GetCurrentDirectory();

                    string inputOGGFile = currentDirectory + "\\" + hash + ".ogg";

                    string outputPCMFile = currentDirectory + "\\" + hash + ".pcm";

                    return_value = RpkgLib.convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

                    if (return_value == 1)
                    {
                        if (File.Exists(outputPCMFile))
                        {
                            if (pcmMemoryStream != null)
                            {
                                pcmMemoryStream.Dispose();
                            }

                            pcmMemoryStream = new MemoryStream(File.ReadAllBytes(outputPCMFile));

                            int pcmSampleSize = RpkgLib.get_pcm_sample_size();
                            int pcmSampleRate = RpkgLib.get_pcm_sample_rate();
                            int pcmChannels = RpkgLib.get_pcm_channels();

                            File.Delete(hash + ".ogg");
                            File.Delete(hash + ".wem");
                            File.Delete(hash + ".pcm");

                            //file = new FileInfo("output.pcm");
                            //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                            waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                            pcmSource = new RawSourceWaveStream(pcmMemoryStream, waveFormat);

                            oggPlayerState = (int)OggPlayerState.READY;
                        }

                        for (int i = 0; i < wwevCount; i++)
                        {
                            OGGPlayerComboBox.Items.Add(i + ".ogg");
                        }

                        OGGPlayerComboBox.SelectedIndex = 0;
                    }
                    else
                    {
                        if (TabAudioPlayer.Visibility == Visibility.Visible)
                        {
                            TabAudioPlayer.Visibility = Visibility.Collapsed;
                        }

                        TabDetails.IsSelected = true;
                    }
                }
                else
                {
                    if (TabAudioPlayer.Visibility == Visibility.Visible)
                    {
                        TabAudioPlayer.Visibility = Visibility.Collapsed;
                    }

                    TabDetails.IsSelected = true;
                }
            }
        }

        private void MainTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            if (!oneOrMoreRPKGsHaveBeenImported || MainTreeView.Nodes.Count == 0)
                return;

            System.Windows.Forms.TreeNode item = e.Node;

            if (item == null)
            {
                return;
            }

            string itemHeader = item.Text;

            currentNodeText = item.Text;

            //WindowUtils.MessageBoxShow(itemHeader);

            rpkgFilePath = GetRootTreeNode(e.Node).Text;

            if (itemHeader.Length == 4) // If the selected item is a resource type.
            {
                OnResourceTypeSelected(itemHeader);
            }
            else if (itemHeader.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase)) // If this is a full rpkg path (top level)
            {
                OnRpkgSelected(itemHeader);
            }
            else // Otherwise it's a file.
            {
                OnResourceSelected(itemHeader);
            }

            //GC.Collect();
        }

        private void MainTreeView_AfterExpand(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            var item = e.Node;

            if (item.Nodes.Count == 1 && item.Nodes[0].Text == "")
            {
                item.Nodes.Clear();

                string[] header = item.Text.Split(' ');

                string resourceType = header[0];

                rpkgFilePath = GetRootTreeNode(e.Node).Text;

                uint hashBasedOnResourceTypeCount = RpkgLib.get_hash_based_on_resource_type_count(rpkgFilePath, resourceType);

                MainTreeView.BeginUpdate();

                int nodeCount = (int)hashBasedOnResourceTypeCount;

                System.Windows.Forms.TreeNode[] nodes = new System.Windows.Forms.TreeNode[nodeCount];

                for (uint i = 0; i < hashBasedOnResourceTypeCount; i++)
                {
                    string hash = "";

                    hash = Marshal.PtrToStringAnsi(RpkgLib.get_hash_based_on_resource_type_at(rpkgFilePath, resourceType, i));

                    string[] temp_test = hash.Split('.');

                    string ioiString = Marshal.PtrToStringAnsi(RpkgLib.get_hash_list_string(temp_test[0]));

                    nodes[i] = new System.Windows.Forms.TreeNode();

                    nodes[i].Text = hash + " " + ioiString;
                }
                                
                item.Nodes.AddRange(nodes);

                MainTreeView.EndUpdate();
            }

            //GC.Collect();
        }

        private void MainTreeView_NodeMouseClick(object sender, System.Windows.Forms.TreeNodeMouseClickEventArgs e)
        {
            if (e.Button != System.Windows.Forms.MouseButtons.Right)
                return;

            MainTreeView.SelectedNode = e.Node;

            //WindowUtils.MessageBoxShow(e.Node.Text);

            //WindowUtils.MessageBoxShow(e.Button.ToString());

            string header = e.Node.Text;

            var point = new Point(e.Location.X, e.Location.Y);

            if (oneOrMoreRPKGsHaveBeenImported)
            {
                if (header.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                {
                    ShowRpkgContextMenu(header, point);
                }
                else if (header.Length == 4)
                {
                    ShowResourceTypeContextMenu(header, GetRootTreeNode(e.Node).Text, point);
                }
                else
                {
                    ShowResourceContextMenu(header, GetRootTreeNode(e.Node).Text, point);
                }
            }
        }

        private void ShowResourceContextMenu(string header, string rpkgPath, Point point)
        {
            TreeNodeMouseClickEventArgs e;
            string[] headerSplit = header.Split(' ');

            string hashName = headerSplit[0];

            rpkgFilePath = rpkgPath;

            //WindowUtils.MessageBoxShow(rpkgFilePath);
            
            string[] hashData = hashName.Split('.');

            string hashValue = hashData[0];
            string hashType = hashData[1];
            
            var actions = ActionRegistry.GetActionsForResourceType(hashType).ToList();

            var buttons = actions.Select(a => a.Description).ToList();
            buttons.Add("Cancel");

            var rightClickMenu = new RightClickMenu(buttons.ToArray());

            rightClickMenu.Left = PointToScreen(point).X;
            rightClickMenu.Top = PointToScreen(point).Y;

            rightClickMenu.ShowDialog();

            if (rightClickMenu.PressedButtonIndex != -1 && rightClickMenu.PressedButtonIndex < actions.Count)
                actions[rightClickMenu.PressedButtonIndex].PerformAction(rpkgFilePath, hashValue);
        }

        private void ShowResourceTypeContextMenu(string header, string rpkgPath, Point point)
        {
            TreeNodeMouseClickEventArgs e;
            rpkgFilePath = rpkgPath;

            var actions = ActionRegistry.GetBulkActionsForResourceType(header).ToList();

            var buttons = actions.Select(a => a.Description).ToList();
            buttons.Add("Cancel");

            var rightClickMenu = new RightClickMenu(buttons.ToArray());

            rightClickMenu.Left = PointToScreen(point).X;
            rightClickMenu.Top = PointToScreen(point).Y;

            rightClickMenu.ShowDialog();

            if (rightClickMenu.PressedButtonIndex != -1 && rightClickMenu.PressedButtonIndex < actions.Count)
                actions[rightClickMenu.PressedButtonIndex].PerformAction(rpkgFilePath, header);
        }

        private void ShowRpkgContextMenu(string header, Point point)
        {
            rpkgFilePath = header;

            string command = "";
            string input_path = rpkgFilePath;
            string filter = "";
            string output_path = App.Settings.OutputFolder;

            Progress progress = new Progress();

            progress.operation = (int) Progress.Operation.MASS_EXTRACT;

            string rpkgFile = rpkgFilePath.Substring(rpkgFilePath.LastIndexOf("\\") + 1);

            string[] buttons = {"Extract All", "Unload " + rpkgFile + " from RPKG", "Cancel"};

            RightClickMenu rightClickMenu = new RightClickMenu(buttons);

            rightClickMenu.Left = PointToScreen(point).X;
            rightClickMenu.Top = PointToScreen(point).Y;

            rightClickMenu.ShowDialog();

            //WindowUtils.MessageBoxShow(rightClickMenu.buttonPressed);
            //WindowUtils.MessageBoxShow(rpkgFilePath);

            if (rightClickMenu.buttonPressed == "button0")
            {
                command = "-extract_from_rpkg";

                Filter filterDialog = new Filter();

                filterDialog.ShowDialog();

                filter = filterDialog.filterString;

                progress.message.Content = "Extracting All Hash Files/Resources...";
            }
            else if (rightClickMenu.buttonPressed == "button1")
            {
                RpkgManager.UnloadRpkg(rpkgFilePath);
                return;
            }
            else
            {
                return;
            }

            int return_value = RpkgLib.reset_task_status();

            RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

            IAsyncResult ar = rpkgExecute.BeginInvoke(
                command,
                input_path,
                filter,
                "",
                "",
                output_path,
                null,
                null
            );

            progress.ShowDialog();

            if (progress.task_status != (int) Progress.RPKGStatus.TASK_SUCCESSFUL)
            {
                //WindowUtils.MessageBoxShow(progress.task_status_string);
            }
        }

        private void HashMapTreeView_AfterExpand(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            var item = e.Node;

            if (item.Nodes.Count == 1 && item.Nodes[0].Text == "")
            {
                item.Nodes.Clear();

                System.Windows.Forms.TreeNode parentPrevious = item.Parent;

                System.Windows.Forms.TreeNode parentCurrent = parentPrevious;

                while (parentCurrent != null)
                {
                    parentPrevious = parentCurrent;

                    parentCurrent = parentPrevious.Parent;
                }

                //System.Windows.MessageBox.Show(parentPrevious.Header.ToString());

                string rpkgFilePathLocal = parentPrevious.Text;

                string[] header = item.Text.Replace("(", "").Replace(")", "").Split(' ');

                string hash = header[0];
                string rpkgFile = header[1];

                //System.Windows.MessageBox.Show(hash + "," + rpkgFilePath);

                int return_value = RpkgLib.reset_task_status();

                RpkgLib.execute_get_direct_hash_depends rpkgExecute = RpkgLib.get_direct_hash_depends;

                IAsyncResult ar = rpkgExecute.BeginInvoke(rpkgFilePathLocal, hash, null, null);

                Progress progress = new Progress();

                progress.operation = (int)Progress.Operation.GENERAL;

                progress.ShowDialog();

                string hashDependsString = Marshal.PtrToStringAnsi(RpkgLib.get_direct_hash_depends_string());

                //System.Windows.MessageBox.Show(hashDependsString);

                string[] hashDepends = hashDependsString.TrimEnd(',').Split(',');

                if (hashDependsString != "")
                {
                    foreach (string hashDepend in hashDepends)
                    {
                        string[] hashData = hashDepend.Split('|');

                        string hash2 = hashData[0];
                        string rpkgFile2 = hashData[1];

                        var item2 = new System.Windows.Forms.TreeNode();

                        string[] temp_test = hash2.Split('.');

                        string ioiString = Marshal.PtrToStringAnsi(RpkgLib.get_hash_list_string(temp_test[0]));

                        item2.Text = hash2 + " (" + rpkgFile + ") " + ioiString;

                        item2.Nodes.Add("");

                        item.Nodes.Add(item2);
                    }
                }
            }

            //GC.Collect();
        }

        private void SearchHashListTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            if (oneOrMoreRPKGsHaveBeenImported)
            {
                if (MainTreeView.Nodes.Count > 0)
                {
                    System.Windows.Forms.TreeNode item = e.Node;

                    if (item == null)
                    {
                        return;
                    }

                    string itemHeader = item.Text;

                    //WindowUtils.MessageBoxShow(itemHeader);

                    string[] header = itemHeader.Split(' ');

                    string hashFile = header[0];

                    string[] hashDetails = hashFile.Split('.');

                    string hash = hashDetails[0];
                    string resourceType = hashDetails[1];

                    string rpkgFilePath = GetRootTreeNode(e.Node).Text;

                    string rpkgHashFilePathString = Marshal.PtrToStringAnsi(RpkgLib.get_rpkg_file_paths_hash_is_in(hash));

                    if (rpkgHashFilePathString != "")
                    {
                        string[] rpkgHashFilePaths = rpkgHashFilePathString.Trim(',').Split(',');

                        ItemDetails.ShowSearchResultDetails(hash, rpkgHashFilePaths);

                        HexViewerTextBox.Text = "";

                        LocalizationTextBox.Text = "";

                        foreach (string rpkgHashFilePath in rpkgHashFilePaths)
                        {
                            //WindowUtils.MessageBoxShow(rpkgHashFilePath);

                            //HexViewerTextBox.Text = "Hex view of " + header[0] + ":\n\n";

                            LocalizationTextBox.Text = "";

                            //HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgHashFilePath, hash));

                            if (RightTabControl.SelectedIndex == 1)
                            {
                                LoadHexEditor();
                            }
                            
                            currentHash = header[0];

                            if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                            {
                                uint localization_data_size = RpkgLib.generate_localization_string(rpkgHashFilePath, hash, resourceType);

                                byte[] localization_data = new byte[localization_data_size];

                                Marshal.Copy(RpkgLib.get_localization_string(), localization_data, 0, (int)localization_data_size);

                                if (localization_data_size > 0)
                                {
                                    LocalizationTextBox.Text = Encoding.UTF8.GetString(localization_data);
                                }

                                if (localization_data_size > 0)
                                {
                                    if (TabJsonViewer.Visibility == Visibility.Collapsed)
                                    {
                                        TabJsonViewer.Visibility = Visibility.Visible;
                                    }
                                }
                                else
                                {
                                    if (TabJsonViewer.Visibility == Visibility.Visible)
                                    {
                                        TabJsonViewer.Visibility = Visibility.Collapsed;
                                    }
                                }
                            }
                            else
                            {
                                if (TabJsonViewer.Visibility == Visibility.Visible)
                                {
                                    TabJsonViewer.Visibility = Visibility.Collapsed;
                                }

                                if (TabImageViewer.Visibility == Visibility.Visible)
                                {
                                    TabImageViewer.Visibility = Visibility.Collapsed;
                                }

                                if (TabAudioPlayer.Visibility == Visibility.Visible)
                                {
                                    TabAudioPlayer.Visibility = Visibility.Collapsed;
                                }

                                if (Tab3dViewer.Visibility == Visibility.Visible)
                                {
                                    Tab3dViewer.Visibility = Visibility.Collapsed;
                                }
                            }

                            int return_value = RpkgLib.clear_hash_data_vector();
                        }
                    }
                    else
                    {
                        ItemDetails.ShowNoSearchResults();

                        HexViewerTextBox.Text = "";

                        LocalizationTextBox.Text = "";
                    }

                    //GC.Collect();
                }
            }
            else
            {
                ItemDetails.ShowNoSearchResults();

                HexViewerTextBox.Text = "";

                LocalizationTextBox.Text = "";
            }
        }

        private void SearchRPKGsTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (searchRPKGsInputTimer == null)
            {
                searchRPKGsInputTimer = new System.Windows.Threading.DispatcherTimer();

                searchRPKGsInputTimer.Interval = TimeSpan.FromMilliseconds(600);

                searchRPKGsInputTimer.Tick += SearchRPKGsTextBox_TimerTimeout;
            }

            searchRPKGsInputTimer.Stop();
            searchRPKGsInputTimer.Tag = (sender as TextBox).Text;
            searchRPKGsInputTimer.Start();
        }

        private void SearchRPKGsTextBox_TimerTimeout(object sender, EventArgs e)
        {
            var timer = (sender as System.Windows.Threading.DispatcherTimer);

            if (timer == null)
            {
                return;
            }

            timer.Stop();

            SearchRPKGsTreeView.Nodes.Clear();

            if (SearchRPKGsTextBox.Text.Length > 0)
            {
                int maxSearchResults = 0;

                int.TryParse(SearchRPKGsComboBox.Text, out maxSearchResults);

                if (maxSearchResults == 0)
                {
                    maxSearchResults = 100;
                }

                int rpkgItemIndex = 0;

                SearchRPKGsTreeView.BeginUpdate();

                foreach (System.Windows.Forms.TreeNode mainTreeNode in MainTreeView.Nodes)
                {
                    string rpkgPath = mainTreeNode.Text;

                    bool rpkgItemAdded = false;

                    //WindowUtils.MessageBoxShow(rpkgPath);

                    int rpkgResultsCount = 0;

                    int resourceTypeItemIndex = 0;

                    foreach (System.Windows.Forms.TreeNode resourceItems in mainTreeNode.Nodes)
                    {
                        if (rpkgResultsCount <= maxSearchResults)
                        {
                            string resourceType = resourceItems.Text;

                            bool resourceTypeItemAdded = false;

                            //WindowUtils.MessageBoxShow(resourceType);

                            int return_value = RpkgLib.search_imported_hashes(SearchRPKGsTextBox.Text, rpkgPath, resourceType, maxSearchResults);

                            //WindowUtils.MessageBoxShow(Marshal.PtrToStringAnsi(get_search_imported_hashes()));

                            string searchResultsString = Marshal.PtrToStringAnsi(RpkgLib.get_search_imported_hashes());

                            bool found = false;

                            System.Windows.Forms.TreeNode item;

                            if (!rpkgItemAdded)
                            {
                                item = new System.Windows.Forms.TreeNode();
                            }
                            else
                            {
                                item = SearchRPKGsTreeView.Nodes[rpkgItemIndex];
                            }

                            item.Text = rpkgPath;

                            //item.IsExpanded = true;

                            string rpkgFile = rpkgPath.Substring(rpkgPath.LastIndexOf("\\") + 1);

                            string[] searchResults = searchResultsString.TrimEnd(',').Split(',');

                            if (searchResults.Length > 0)
                            {
                                if (searchResults[0] != "")
                                {
                                    int nodeCount = 0;

                                    System.Windows.Forms.TreeNode[] nodes = new System.Windows.Forms.TreeNode[searchResults.Length];

                                    System.Windows.Forms.TreeNode item2;

                                    if (!resourceTypeItemAdded)
                                    {
                                        item2 = new System.Windows.Forms.TreeNode();

                                        item2.Text = resourceType;

                                        item2.Expand();
                                    }
                                    else
                                    {
                                        item2 = item.Nodes[resourceTypeItemIndex];
                                    }

                                    foreach (string searchResult in searchResults)
                                    {
                                        found = true;

                                        rpkgResultsCount++;

                                        //WindowUtils.MessageBoxShow(searchResult);

                                        //var item3 = new System.Windows.Forms.TreeNode();

                                        string[] temp_test = searchResult.Split('.');

                                        string ioiString = Marshal.PtrToStringAnsi(RpkgLib.get_hash_list_string(temp_test[0]));

                                        //item3.Header = searchResult + " (" + rpkgFile + ") " + ioiString;

                                        //item2.Nodes.Add(searchResult + " (" + rpkgFile + ") " + ioiString);

                                        nodes[nodeCount] = new System.Windows.Forms.TreeNode();

                                        nodes[nodeCount].Text = searchResult + " (" + rpkgFile + ") " + ioiString;

                                        //item3.Expanded += Item_Expanded;

                                        //item2.Items.Add(item3);

                                        nodeCount++;
                                    }

                                    item2.Nodes.AddRange(nodes);

                                    if (!resourceTypeItemAdded)
                                    {
                                        item.Nodes.Add(item2);

                                        resourceTypeItemAdded = true;
                                    }

                                    if (!rpkgItemAdded && found)
                                    {
                                        SearchRPKGsTreeView.Nodes.Add(item);

                                        rpkgItemAdded = true;
                                    }
                                }
                            }

                            if (resourceTypeItemAdded)
                            {
                                resourceTypeItemIndex++;
                            }
                        }
                    }

                    if (rpkgItemAdded)
                    {
                        rpkgItemIndex++;
                    }
                }

                SearchRPKGsTreeView.EndUpdate();
            }
        }

        private void SearchHashListTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (searchHashListInputTimer == null)
            {
                searchHashListInputTimer = new System.Windows.Threading.DispatcherTimer();

                searchHashListInputTimer.Interval = TimeSpan.FromMilliseconds(600);

                searchHashListInputTimer.Tick += SearchHashListTextBox_TimerTimeout;
            }

            searchHashListInputTimer.Stop();
            searchHashListInputTimer.Tag = (sender as TextBox).Text;
            searchHashListInputTimer.Start();
        }

        private void SearchHashListTextBox_TimerTimeout(object sender, EventArgs e)
        {
            var timer = (sender as System.Windows.Threading.DispatcherTimer);

            if (timer == null)
            {
                return;
            }

            timer.Stop();

            SearchHashListTreeView.Nodes.Clear();

            SearchHashListTreeView.BeginUpdate();

            if (SearchHashListTextBox.Text.Length > 0)
            {
                int maxSearchResults = 0;

                int.TryParse(SearchHashListComboBox.Text, out maxSearchResults);

                if (maxSearchResults == 0)
                {
                    maxSearchResults = 100;
                }

                int return_value = RpkgLib.search_hash_list(SearchHashListTextBox.Text, maxSearchResults);

                string searchResultsString = Marshal.PtrToStringAnsi(RpkgLib.get_search_hash_list());

                string[] searchResults = searchResultsString.TrimEnd(',').Split(',');

                if (searchResults.Length > 0)
                {
                    if (searchResults[0] != "")
                    {
                        int nodeCount = 0;

                        System.Windows.Forms.TreeNode[] nodes = new System.Windows.Forms.TreeNode[searchResults.Length];

                        foreach (string searchResult in searchResults)
                        {
                            string[] temp_test = searchResult.Split('.');

                            string ioiString = Marshal.PtrToStringAnsi(RpkgLib.get_hash_list_string(temp_test[0]));

                            //SearchHashListTreeView.Nodes.Add(searchResult + " (hashlist) " + ioiString);

                            nodes[nodeCount] = new System.Windows.Forms.TreeNode();

                            nodes[nodeCount].Text = searchResult + " (hashlist) " + ioiString;

                            nodeCount++;
                        }

                        SearchHashListTreeView.Nodes.AddRange(nodes);
                    }
                }
            }

            SearchHashListTreeView.EndUpdate();
        }

        private void LeftTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            TabControl tab = (sender as TabControl);

            //WindowUtils.MessageBoxShow(tab.SelectedIndex.ToString());

            if (tab.SelectedIndex == 0)
            {
                SetDiscordStatus("Resource View", "");
            }
            else if (tab.SelectedIndex == 1)
            {
                LoadHashDependsMap();

                SetDiscordStatus("Dependency View", "");
            }
            else if (tab.SelectedIndex == 2)
            {
                SetDiscordStatus("Search View", "");
            }
        }

        private void RightTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            TabControl tab = (sender as TabControl);

            //WindowUtils.MessageBoxShow(tab.SelectedIndex.ToString());

            if (tab.SelectedIndex == 1)
            {
                LoadHexEditor();
            }
        }

        private System.Windows.Forms.TreeNode GetRootTreeNode(System.Windows.Forms.TreeNode node)
        {
            System.Windows.Forms.TreeNode parentNode = node;

            while (parentNode.Parent != null)
            {
                parentNode = parentNode.Parent;
            }

            return parentNode;
        }

        private void LoadHexEditor()
        {
            HexViewerTextBox.Text = "Hex view of " + currentHash + ":\n\n";

            string[] hashDetails = currentHash.Split('.');

            string hash = hashDetails[0];

            HexViewerTextBox.Text += Marshal.PtrToStringAnsi(RpkgLib.get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash));
        }

        public static void TEXTToPNGThread(string rpkgFilePathLocal, string hash, string png_file_name)
        {
            int return_value = RpkgLib.generate_png_from_text(rpkgFilePathLocal, hash, png_file_name);
        }

        public static void MassExtractTEXTThread(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            int return_value = RpkgLib.task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public static int MassExtractTEXT(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            Thread thread = new Thread(() => MassExtractTEXTThread(command, input_path, filter, search, search_type, output_path));
            thread.SetApartmentState(ApartmentState.MTA);
            thread.Start();
            thread.Join();
            thread = null;

            return 0;
        }

        public static void RebuildTEXTThread(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            int return_value = RpkgLib.task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public static int RebuildTEXT(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            Thread thread = new Thread(() => RebuildTEXTThread(command, input_path, filter, search, search_type, output_path));
            thread.SetApartmentState(ApartmentState.MTA);
            thread.Start();
            thread.Join();
            thread = null;

            return 0;
        }

        public static void ExtractTEXTThread(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            int return_value = RpkgLib.task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public static int ExtractTEXT(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            Thread thread = new Thread(() => ExtractTEXTThread(command, input_path, filter, search, search_type, output_path));
            thread.SetApartmentState(ApartmentState.MTA);
            thread.Start();
            thread.Join();
            thread = null;

            return 0;
        }

        public static void ExtractMODELThread(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            int return_value = RpkgLib.task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public static int ExtractMODEL(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            Thread thread = new Thread(() => ExtractMODELThread(command, input_path, filter, search, search_type, output_path));
            thread.SetApartmentState(ApartmentState.MTA);
            thread.Start();
            thread.Join();
            thread = null;

            return 0;
        }

        public static void RebuildMODELThread(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            int return_value = RpkgLib.task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public static int RebuildMODEL(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            Thread thread = new Thread(() => RebuildMODELThread(command, input_path, filter, search, search_type, output_path));
            thread.SetApartmentState(ApartmentState.MTA);
            thread.Start();
            thread.Join();
            thread = null;

            return 0;
        }

        private void OpenRPKGFile_Click(object sender, RoutedEventArgs e)
        {
            var fileDialog = new VistaOpenFileDialog();

            fileDialog.Title = "Select an RPKG file to import:";

            fileDialog.Filter = "RPKG file|*.rpkg|All files|*.*";

            if (!Directory.Exists(App.Settings.InputFolder))
            {
                App.Settings.InputFolder = Directory.GetCurrentDirectory();

                var options = new JsonSerializerOptions { WriteIndented = true };

                string jsonString = JsonSerializer.Serialize(App.Settings, options);

                File.WriteAllText("rpkg.json", jsonString);
            }

            fileDialog.InitialDirectory = App.Settings.InputFolder;

            fileDialog.FileName = App.Settings.InputFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                RpkgManager.LoadRpkg(fileDialog.FileName);

                //LoadHashDependsMap();
            }
        }

        private void Extract_Click(object sender, RoutedEventArgs e)
        {
            var fileDialog = new VistaOpenFileDialog();

            fileDialog.Title = "Select an RPKG file to extract from:";

            fileDialog.Filter = "RPKG file|*.rpkg|All files|*.*";

            string initialFolder = "";

            if (File.Exists(App.Settings.InputFolder))
            {
                initialFolder = App.Settings.InputFolder;
            }
            else
            {
                initialFolder = Directory.GetCurrentDirectory();
            }

            fileDialog.InitialDirectory = initialFolder;

            fileDialog.FileName = initialFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                RpkgManager.LoadRpkg(fileDialog.FileName);

                //LoadHashDependsMap();
            }
            else
            {
                return;
            }

            string outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract RPKG To:");

            if (outputFolder == "")
            {
                return;
            }

            string command = "-extract_from_rpkg";
            string input_path = fileDialog.FileName;
            string filter = "";
            string search = "";
            string search_type = "";
            string output_path = outputFolder;

            Progress progress = new Progress();

            progress.operation = (int)Progress.Operation.MASS_EXTRACT;

            Filter filterDialog = new Filter();

            filterDialog.ShowDialog();

            filter = filterDialog.filterString;

            int return_value = RpkgLib.reset_task_status();

            RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

            IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

            progress.ShowDialog();

            if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
            {
                //WindowUtils.MessageBoxShow(progress.task_status_string);
            }
        }

        private void MassExtract_Click(object sender, RoutedEventArgs e)
        {
            string massExtractButton = (sender as MenuItem).Header.ToString();

            string massExtractName = "";

            if (massExtractButton.Contains("ORES"))
            {
                massExtractName = "ORES";
            }
            else if (massExtractButton.Contains("WWEM"))
            {
                massExtractName = "WWEM";
            }
            else if (massExtractButton.Contains("WWES"))
            {
                massExtractName = "WWES";
            }
            else if (massExtractButton.Contains("WWEV"))
            {
                massExtractName = "WWEV";
            }
            else if (massExtractButton.Contains("DLGE"))
            {
                massExtractName = "DLGE";
            }
            else if (massExtractButton.Contains("LOCR"))
            {
                massExtractName = "LOCR";
            }
            else if (massExtractButton.Contains("RTLV"))
            {
                massExtractName = "RTLV";
            }
            else if (massExtractButton.Contains("GFXF"))
            {
                massExtractName = "GFXF";
            }
            else if (massExtractButton.Contains("TEXT"))
            {
                massExtractName = "TEXT";
            }
            else if (massExtractButton.Contains("PRIM"))
            {
                massExtractName = "PRIM";
            }

            string inputFolder = WindowUtils.SelectFolder("input", "Select Input Folder (Runtime folder or other folder with multiple RPKGs) To Extract " + massExtractName + " From:");

            if (inputFolder == "")
            {
                return;
            }

            /*List<string> rpkgFiles = new List<string>();

            foreach (var filePath in Directory.GetFiles(inputFolder))
            {
                if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                {
                    rpkgFiles.Add(filePath);
                }
            }

            rpkgFiles.Sort(new NaturalStringComparer());

            bool anyRPKGImported = false;

            foreach (string filePath in rpkgFiles)
            {
                ImportRPKGFile(filePath);

                anyRPKGImported = true;
            }

            if (anyRPKGImported)
            {
                //LoadHashDependsMap();
            }*/

            RpkgManager.LoadRpkgsFromFolder(inputFolder);

            string outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + massExtractName + " To:");

            if (outputFolder == "")
            {
                return;
            }

            string command = "";
            string input_path = inputFolder;
            string filter = "";
            string search = "";
            string search_type = "";
            string output_path = outputFolder;

            Progress progress = new Progress();

            progress.operation = (int)Progress.Operation.MASS_EXTRACT;

            Filter filterDialog = new Filter();

            filterDialog.message1.Content = "Enter extraction filter for " + massExtractName + " below.";

            if (massExtractName == "TEXT")
            {
                command = "-extract_all_text_from";

                progress.message.Content = "Extracting TEXT/TEXD...";

                int return_value = RpkgLib.reset_task_status();

                RpkgLib.execute_task rpkgExecute = MassExtractTEXT;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                progress.ShowDialog();

                if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                {
                    //WindowUtils.MessageBoxShow(progress.task_status_string);
                }
            }
            else
            {
                if (massExtractName == "ORES")
                {
                    filterDialog.ShowDialog();

                    filter = filterDialog.filterString;

                    command = "-extract_ores_from";

                    progress.ProgressBar.IsIndeterminate = true;

                    progress.message.Content = "Extracting ORES To IOI Paths...";
                }
                else if (massExtractName == "WWEM")
                {
                    filterDialog.ShowDialog();

                    filter = filterDialog.filterString;

                    command = "-extract_wwem_to_ogg_from";

                    progress.message.Content = "Extracting WWEM To IOI Paths...";
                }
                else if (massExtractName == "WWES")
                {
                    filterDialog.ShowDialog();

                    filter = filterDialog.filterString;

                    command = "-extract_wwes_to_ogg_from";

                    progress.message.Content = "Extracting WWES To IOI Paths...";
                }
                else if (massExtractName == "WWEV")
                {
                    filterDialog.ShowDialog();

                    filter = filterDialog.filterString;

                    command = "-extract_wwev_to_ogg_from";

                    progress.message.Content = "Extracting WWEV To IOI Paths...";
                }
                else if (massExtractName == "DLGE")
                {
                    command = "-extract_dlge_to_json_from";

                    progress.message.Content = "Extracting DLGEs To JSONs...";
                }
                else if (massExtractName == "LOCR")
                {
                    command = "-extract_locr_to_json_from";

                    progress.message.Content = "Extracting LOCRs To JSONs...";
                }
                else if (massExtractName == "RTLV")
                {
                    command = "-extract_rtlv_to_json_from";

                    progress.message.Content = "Extracting RTLVs To JSONs...";
                }
                else if (massExtractName == "GFXF")
                {
                    command = "-extract_gfxf_from";

                    progress.message.Content = "Extracting GFXF...";
                }
                else if (massExtractName == "PRIM")
                {
                    command = "-extract_all_prim_to_glb_from";

                    progress.message.Content = "Extracting PRIM...";
                }

                int return_value = RpkgLib.reset_task_status();

                RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                progress.ShowDialog();

                if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                {
                    //WindowUtils.MessageBoxShow(progress.task_status_string);
                }
            }
        }

        private void Rebuild_Click(object sender, RoutedEventArgs e)
        {
            string rebuildButton = (sender as MenuItem).Header.ToString();

            string rebuildType = "";

            if (rebuildButton.Contains("DLGE"))
            {
                rebuildType = "DLGE";
            }
            else if (rebuildButton.Contains("GFXF"))
            {
                rebuildType = "GFXF";
            }
            else if (rebuildButton.Contains("LOCR"))
            {
                rebuildType = "LOCR";
            }
            else if (rebuildButton.Contains("RTLV"))
            {
                rebuildType = "RTLV";
            }
            else if (rebuildButton.Contains("PRIM/TEXT/TEXD"))
            {
                rebuildType = "PRIM/TEXT/TEXD";
            }
            else if (rebuildButton.Contains("TEXT"))
            {
                rebuildType = "TEXT";
            }
            else if (rebuildButton.Contains("PRIM"))
            {
                rebuildType = "PRIM";
            }
            else if (rebuildButton.Contains("WWEV"))
            {
                rebuildType = "WWEV";
            }

            string inputFolder = WindowUtils.SelectFolder("input", "Select Folder To Rebuild " + rebuildType + " From:");

            if (inputFolder != "")
            {
                string command = "";
                string input_path = inputFolder;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = App.Settings.OutputFolder;

                Progress progress = new Progress();

                progress.operation = (int)Progress.Operation.GENERAL;

                progress.ProgressBar.IsIndeterminate = true;

                if (rebuildType == "PRIM/TEXT/TEXD")
                {
                    command = "-rebuild_prim_model_in";

                    progress.message.Content = "Rebuilding All PRIM Models in " + input_path + "...";

                    int return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task rpkgExecute = RebuildMODEL;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.operation = (int)Progress.Operation.PRIM_MODEL_REBUILD;

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.PRIM_MODEL_REBUILD_SUCCESSFUL)
                    {
                        WindowUtils.MessageBoxShow(progress.task_status_string.Replace("_", "__"));

                        /*if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_GLB_MESH_NAME_MALFORMED)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB mesh name is malformed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED)
                        {
                            WindowUtils.MessageBoxShow("Error: Only one mesh primitive per mesh is allowed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB vertex float size is not a multiple of 3.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_POSITION_DATA)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is missing POSITION data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISMATCHED_BONES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB has mismatched bones compared to the original BORG file.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_MISSING)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is weighted, but does not have all necessary weighted data: JOINTS_0, WEIGHTS_0, JOINTS_1, WEIGHTS_1.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB vertex float size not equal to normal float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_NORMAL_DATA)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is missing NORMAL data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB vertex float size not equal to UV float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_UV_DATA)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is missing TEXCOORD_0 data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB color size not equal to vertex size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_WRONG_FORMAT)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS)
                        {
                            WindowUtils.MessageBoxShow("Error: PRIM has too many primary object headers.");
                        }*/

                        return_value = RpkgLib.clear_temp_tblu_data();
                    }
                    else
                    {
                        WindowUtils.MessageBoxShow("PRIM model(s) rebuilt successfully in " + inputFolder);
                    }
                }
                else if (rebuildType == "TEXT")
                {
                    command = "-rebuild_text_in";

                    progress.message.Content = "Rebuilding All TEXT/TEXD in " + input_path + "...";

                    int return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task rpkgExecute = RebuildTEXT;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //WindowUtils.MessageBoxShow(progress.task_status_string);
                    }
                }
                else if (rebuildType == "PRIM")
                {
                    command = "-rebuild_prim_in";

                    progress.message.Content = "Rebuilding All PRIM in " + input_path + "...";

                    int return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task rpkgExecute = RebuildMODEL;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.operation = (int)Progress.Operation.PRIM_REBUILD;

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.PRIM_REBUILD_SUCCESSFUL)
                    {
                        WindowUtils.MessageBoxShow(progress.task_status_string.Replace("_", "__"));

                        /*if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_GLB_MESH_NAME_MALFORMED)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB mesh name is malformed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED)
                        {
                            WindowUtils.MessageBoxShow("Error: Only one mesh primitive per mesh is allowed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB vertex float size is not a multiple of 3.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_POSITION_DATA)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is missing POSITION data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISMATCHED_BONES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB has mismatched bones compared to the original BORG file.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_MISSING)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is weighted, but does not have all necessary weighted data: JOINTS_0, WEIGHTS_0, JOINTS_1, WEIGHTS_1.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB vertex float size not equal to normal float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_NORMAL_DATA)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is missing NORMAL data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB vertex float size not equal to UV float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_UV_DATA)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB is missing TEXCOORD_0 data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB color size not equal to vertex size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_WRONG_FORMAT)
                        {
                            WindowUtils.MessageBoxShow("Error: GLB color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS)
                        {
                            WindowUtils.MessageBoxShow("Error: PRIM has too many primary object headers.");
                        }*/

                        return_value = RpkgLib.clear_temp_tblu_data();
                    }
                    else
                    {
                        WindowUtils.MessageBoxShow("PRIM(s) rebuilt successfully in " + inputFolder);
                    }
                }
                else
                {
                    if (rebuildType == "DLGE")
                    {
                        command = "-rebuild_dlge_from_json_from";

                        progress.message.Content = "Rebuilding All DLGE from JSON in " + input_path + "...";
                    }
                    else if (rebuildType == "GFXF")
                    {
                        command = "-rebuild_gfxf_in";

                        progress.message.Content = "Rebuilding All GFXF in " + input_path + "...";
                    }
                    else if (rebuildType == "LOCR")
                    {
                        command = "-rebuild_locr_from_json_from";

                        progress.message.Content = "Rebuilding All LOCR from JSON in " + input_path + "...";
                    }
                    else if (rebuildType == "RTLV")
                    {
                        command = "-rebuild_rtlv_from_json_from";

                        progress.message.Content = "Rebuilding All RTLV from JSON in " + input_path + "...";
                    }
                    else if (rebuildType == "WWEV")
                    {
                        command = "-rebuild_wwev_in";

                        progress.message.Content = "Rebuilding All WWEV in " + input_path + "...";
                    }

                    int return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        WindowUtils.MessageBoxShow(progress.task_status_string);
                    }
                }
            }
        }

        private void OpenRPKGFolder_Click(object sender, RoutedEventArgs e)
        {
            string inputFolder = WindowUtils.SelectFolder("input", "Select Folder Containing RPKG Files To Import From:");

            if (inputFolder != "")
            {
                RpkgManager.LoadRpkgsFromFolder(inputFolder);

                /*List<string> rpkgFiles = new List<string>();

                foreach (var filePath in Directory.GetFiles(inputFolder))
                {
                    if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                    {
                        rpkgFiles.Add(filePath);
                    }
                }

                rpkgFiles.Sort(new NaturalStringComparer());

                bool anyRPKGImported = false;

                foreach (string filePath in rpkgFiles)
                {
                    ImportRPKGFile(filePath);

                    anyRPKGImported = true;
                }

                if (anyRPKGImported)
                {
                    //LoadHashDependsMap();
                }*/
            }
        }

        private void GenerateRPKGFromFolder_Click(object sender, RoutedEventArgs e)
        {
            string inputFolder = WindowUtils.SelectFolder("input", "Select Folder To Generate RPKG From:");

            if (inputFolder != "")
            {
                string outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Save Generated RPKG To:");

                string command = "-generate_rpkg_from";
                string input_path = inputFolder;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = outputFolder;

                string rpkgFile = inputFolder.Substring(inputFolder.LastIndexOf("\\") + 1);

                RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                Progress progress = new Progress();

                progress.message.Content = "Generating RPKG file " + rpkgFile + "...";

                progress.ShowDialog();
            }
        }
        
        private void ChangeColorTheme_Click(object sender, RoutedEventArgs e)
        {
            var button = (sender as MenuItem);

            string[] header = (button.Header as string).Split('/');

            string brightness = header[0];
            string color = header[1];

            SetColorTheme(brightness, color);

            App.Settings.ColorTheme = (button.Header as string);

            var options = new JsonSerializerOptions { WriteIndented = true };

            string jsonString = JsonSerializer.Serialize(App.Settings, options);

            File.WriteAllText("rpkg.json", jsonString);
        }

        private void LoadHashDependsMap()
        {
            if (oneOrMoreRPKGsHaveBeenImported)
            {
                foreach (System.Windows.Forms.TreeNode rpkgItem in MainTreeView.Nodes)
                {
                    string rpkgPath = rpkgItem.Text;

                    bool alreadyLoaded = false;

                    HashMapTreeView.AfterExpand += HashMapTreeView_AfterExpand;

                    foreach (System.Windows.Forms.TreeNode treeViewNode in HashMapTreeView.Nodes)
                    {
                        if (rpkgPath == treeViewNode.Text)
                        {
                            alreadyLoaded = true;
                        }
                    }

                    if (!alreadyLoaded)
                    {
                        var item = new System.Windows.Forms.TreeNode();

                        item.Text = rpkgPath;

                        int return_value = RpkgLib.reset_task_status();

                        RpkgLib.execute_get_hashes_with_no_reverse_depends rpkg = RpkgLib.get_hashes_with_no_reverse_depends;

                        IAsyncResult ar = rpkg.BeginInvoke(rpkgPath, null, null);

                        Progress progress = new Progress();

                        progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                        progress.ShowDialog();

                        string hashesWithNoReveserseDepends = Marshal.PtrToStringAnsi(RpkgLib.get_hashes_with_no_reverse_depends_string());

                        string[] hashes = hashesWithNoReveserseDepends.TrimEnd(',').Split(',');

                        string rpkgFile = rpkgPath.Substring(rpkgPath.LastIndexOf("\\") + 1);

                        foreach (string hash in hashes)
                        {
                            var item2 = new System.Windows.Forms.TreeNode();

                            string[] temp_test = hash.Split('.');

                            string ioiString = Marshal.PtrToStringAnsi(RpkgLib.get_hash_list_string(temp_test[0]));

                            item2.Text = hash + " (" + rpkgFile + ") " + ioiString;

                            item2.Nodes.Add("");

                            item.Nodes.Add(item2);
                        }

                        HashMapTreeView.Nodes.Add(item);
                    }
                }
            }
        }

        private void SetColorTheme(string brightness, string color)
        {
            if (brightness == "Light" && color == "Blue")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.DodgerBlue)));
            if (brightness == "Light" && color == "Brown")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.BurlyWood)));
            if (brightness == "Light" && color == "Green")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Green)));
            if (brightness == "Light" && color == "Orange")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Orange)));
            if (brightness == "Light" && color == "Purple")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Purple)));
            if (brightness == "Light" && color == "Red")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Red)));
            if (brightness == "Light" && color == "Yellow")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Yellow)));
            if (brightness == "Light" && color == "White")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.White)));
            if (brightness == "Dark" && color == "Blue")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.DodgerBlue)));
            if (brightness == "Dark" && color == "Brown")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.BurlyWood)));
            if (brightness == "Dark" && color == "Green")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Green)));
            if (brightness == "Dark" && color == "Orange")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Orange)));
            if (brightness == "Dark" && color == "Purple")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Purple)));
            if (brightness == "Dark" && color == "Red")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Red)));
            if (brightness == "Dark" && color == "Yellow")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Yellow)));
            if (brightness == "Dark" && color == "White")
                ThemeManager.Current.ChangeTheme(Application.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.White)));

            if (brightness == "Dark")
            {
                MainTreeView.ForeColor = ColorTranslator.FromHtml("#FFFFFF");
                MainTreeView.BackColor = ColorTranslator.FromHtml("#252525");
                HashMapTreeView.ForeColor = ColorTranslator.FromHtml("#FFFFFF");
                HashMapTreeView.BackColor = ColorTranslator.FromHtml("#252525");
                SearchRPKGsTreeView.ForeColor = ColorTranslator.FromHtml("#FFFFFF");
                SearchRPKGsTreeView.BackColor = ColorTranslator.FromHtml("#252525");
                SearchHashListTreeView.ForeColor = ColorTranslator.FromHtml("#FFFFFF");
                SearchHashListTreeView.BackColor = ColorTranslator.FromHtml("#252525");
            }
            else if (brightness == "Light")
            {
                MainTreeView.ForeColor = ColorTranslator.FromHtml("#000000");
                MainTreeView.BackColor = ColorTranslator.FromHtml("#FFFFFF");
                HashMapTreeView.ForeColor = ColorTranslator.FromHtml("#000000");
                HashMapTreeView.BackColor = ColorTranslator.FromHtml("#FFFFFF");
                SearchRPKGsTreeView.ForeColor = ColorTranslator.FromHtml("#000000");
                SearchRPKGsTreeView.BackColor = ColorTranslator.FromHtml("#FFFFFF");
                SearchHashListTreeView.ForeColor = ColorTranslator.FromHtml("#000000");
                SearchHashListTreeView.BackColor = ColorTranslator.FromHtml("#FFFFFF");
            }
        }

        private void About_Click(object sender, RoutedEventArgs e)
        {
            Licenses licenses = new Licenses();
            licenses.ShowDialog();
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            if (discordOn)
            {
                Client.Dispose();
            }

            Close();
        }
        
        private bool oneOrMoreRPKGsHaveBeenImported;
        private string rpkgFilePath = "";
        private string currentHash = "";
        private string currentHashFileName = "";
        public string hashDependsRPKGFilePath = "";
        private string currentNodeText = "";
        private System.Windows.Threading.DispatcherTimer searchRPKGsInputTimer;
        private System.Windows.Threading.DispatcherTimer searchHashListInputTimer;
        private System.Windows.Threading.DispatcherTimer OGGPlayerTimer;
        private WaveOut waveOut;
        private MemoryStream pcmMemoryStream;
        private int pcmSampleSize;
        private int pcmSampleRate;
        private int pcmChannels;
        public EntityBrickEditor entityBrickEditor;
        public bool discordOn;
        public Timestamps timestamp;

        private enum OggPlayerState
        {
            NULL,
            READY,
            PLAYING,
            PAUSED,
            RESET
        }

        private int oggPlayerState = (int)OggPlayerState.NULL;
        private bool oggPlayerRunning = false;
        private bool oggPlayerPaused;
        private bool oggPlayerStopped;
        private bool oggPlayerStoppedNew;
        FileInfo file;
        FileStream stream;
        WaveFormat waveFormat;
        RawSourceWaveStream pcmSource;
        
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            Process.GetCurrentProcess().Kill();
        }

        private void OGGPlayerTimer_Tick(object sender, EventArgs e)
        {
            //WindowUtils.MessageBoxShow((((double)waveStream.Position / (double)waveStream.Length) * 100.0).ToString());
            OGGPlayer.Value = (pcmSource.Position / (double)pcmSource.Length) * 100.0;
            OGGPlayerLabel.Content = pcmSource.CurrentTime + " / " + pcmSource.TotalTime;

            if (pcmSource.Position == pcmSource.Length)
            {
                if (OGGPlayerTimer == null)
                {
                    return;
                }

                OGGPlayerTimer.Stop();

                waveOut.Stop();

                //WindowUtils.MessageBoxShow(waveStream.CurrentTime.ToString());

                OGGPlayer.Value = 0;

                pcmSource.Position = 0;

                oggPlayerPaused = false;

                oggPlayerStopped = true;

                OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline;

                if (OGGPlayerComboBox.Items.Count > 1)
                {
                    oggPlayerState = (int)OggPlayerState.NULL;

                    int newIndex = OGGPlayerComboBox.SelectedIndex + 1;

                    if (newIndex >= OGGPlayerComboBox.Items.Count)
                    {
                        newIndex = 0;
                    }

                    OGGPlayerComboBox.SelectedIndex = newIndex;

                    string[] hashData = OGGPlayerLabelHashFileName.Content.ToString().Split('.');

                    int return_value = RpkgLib.create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hashData[0], 1, newIndex);

                    string currentDirectory = Directory.GetCurrentDirectory();

                    string inputOGGFile = currentDirectory + "\\" + hashData[0] + ".ogg";

                    string outputPCMFile = currentDirectory + "\\" + hashData[0] + ".pcm";

                    return_value = RpkgLib.convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

                    if (return_value == 1)
                    {
                        if (File.Exists(outputPCMFile))
                        {
                            if (OGGPlayerTimer != null)
                            {
                                OGGPlayerTimer.Stop();
                            }

                            if (waveOut != null)
                            {
                                waveOut.Stop();
                                waveOut.Dispose();
                                waveOut = null;
                            }

                            if (pcmMemoryStream != null)
                            {
                                pcmMemoryStream.Dispose();
                            }

                            pcmMemoryStream = new MemoryStream(File.ReadAllBytes(outputPCMFile));

                            int pcmSampleSize = RpkgLib.get_pcm_sample_size();
                            int pcmSampleRate = RpkgLib.get_pcm_sample_rate();
                            int pcmChannels = RpkgLib.get_pcm_channels();

                            File.Delete(hashData[0] + ".ogg");
                            File.Delete(hashData[0] + ".wem");
                            File.Delete(hashData[0] + ".pcm");

                            //file = new FileInfo("output.pcm");
                            //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                            waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                            pcmSource = new RawSourceWaveStream(pcmMemoryStream, waveFormat);

                            oggPlayerState = (int)OggPlayerState.READY;
                        }
                    }
                    else
                    {
                        if (TabAudioPlayer.Visibility == Visibility.Visible)
                        {
                            TabAudioPlayer.Visibility = Visibility.Collapsed;
                        }

                        TabDetails.IsSelected = true;
                    }
                }
                else
                {
                    oggPlayerState = (int)OggPlayerState.RESET;
                }
            }

            //WindowUtils.MessageBoxShow(pcmSource.Position.ToString() + "," + pcmSource.Length.ToString());
            //WindowUtils.MessageBoxShow(pcmSource.CurrentTime.ToString() + " / " + pcmSource.TotalTime.ToString());
        }

        private void OGGPlayer_DragCompleted(object sender, System.Windows.Controls.Primitives.DragCompletedEventArgs e)
        {
            //WindowUtils.MessageBoxShow(waveStream.Length.ToString() + "," + ((double)waveStream.Length * (OGGPlayer.Value / 100.0)).ToString() + "," + OGGPlayer.Value.ToString());

            //WindowUtils.MessageBoxShow(waveStream.CurrentTime.ToString());

            pcmSource.Position = (long)(pcmSource.Length * (OGGPlayer.Value / 100.0));

            OGGPlayerTimer.Start();
        }

        private void OGGPlayer_DragStarted(object sender, System.Windows.Controls.Primitives.DragStartedEventArgs e)
        {
            if (OGGPlayerTimer == null)
            {
                return;
            }

            OGGPlayerTimer.Stop();
        }

        private void OGGPlayer_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (OGGPlayerTimer == null)
            {
                return;
            }

            OGGPlayerTimer.Stop();

            //WindowUtils.MessageBoxShow(waveStream.CurrentTime.ToString());

            pcmSource.Position = (long)(pcmSource.Length * (OGGPlayer.Value / 100.0));

            OGGPlayerTimer.Start();
        }

        private void AddHandlers()
        {
            OGGPlayer.AddHandler(
                PreviewMouseLeftButtonDownEvent,
                new MouseButtonEventHandler((sender, e) =>
                {
                    if (!OGGPlayer.IsMoveToPointEnabled ||
                    !(OGGPlayer.Template.FindName("PART_Track", OGGPlayer) is System.Windows.Controls.Primitives.Track track) ||
                    track.Thumb?.IsMouseOver != false)
                    {
                        return;
                    }
                    track.Thumb.UpdateLayout();
                    track.Thumb.RaiseEvent(new MouseButtonEventArgs(e.MouseDevice, e.Timestamp, MouseButton.Left)
                    {
                        RoutedEvent = MouseLeftButtonDownEvent,
                        Source = track.Thumb
                    });
                }), true);
        }

        private void PackIconMaterialDesign_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (OGGPlayerTimer != null)
            {
                OGGPlayerTimer.Stop();
            }

            if (OGGPlayerPlay.Kind == MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline)
            {
                if ((oggPlayerState == (int)OggPlayerState.PAUSED || oggPlayerState == (int)OggPlayerState.RESET) && waveOut != null)
                {
                    if (oggPlayerState == (int)OggPlayerState.RESET)
                    {
                        waveOut.Play();

                        OGGPlayerTimer.Start();
                    }
                    else
                    {
                        waveOut.Resume();

                        OGGPlayerTimer.Start();
                    }

                    oggPlayerPaused = true;

                    OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PauseCircleOutline;
                }
                else
                {
                    if (OGGPlayerTimer == null)
                    {
                        OGGPlayerTimer = new System.Windows.Threading.DispatcherTimer();

                        OGGPlayerTimer.Interval = TimeSpan.FromMilliseconds(200);

                        OGGPlayerTimer.Tick += OGGPlayerTimer_Tick;
                    }

                    OGGPlayerTimer.Stop();
                    OGGPlayerTimer.Start();

                    oggPlayerStoppedNew = false;

                    waveOut = new WaveOut();
                    MultiplexingWaveProvider multiplexingWaveProvider;
                    multiplexingWaveProvider = new MultiplexingWaveProvider(new IWaveProvider[] { pcmSource }, 2);
                    multiplexingWaveProvider.ConnectInputToOutput(0, 0);
                    multiplexingWaveProvider.ConnectInputToOutput(0, 1);
                    var wait = new ManualResetEventSlim(false);
                    waveOut.PlaybackStopped += (s, ee) => wait.Set();

                    try
                    {
                        //if (pcmSource.WaveFormat.Channels > 2)
                        //{
                        waveOut.Init(multiplexingWaveProvider);
                        //}
                        //else
                        //{
                        //waveOut.Init(waveStream);
                        //}
                        waveOut.Play();

                        oggPlayerState = (int)OggPlayerState.PLAYING;
                    }
                    catch
                    {
                        WindowUtils.MessageBoxShow("OGG File can't be played due to WaveBadFormat error.");
                    }

                    OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PauseCircleOutline;
                }
            }
            else if (OGGPlayerPlay.Kind == MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PauseCircleOutline)
            {
                waveOut.Pause();

                oggPlayerPaused = true;

                oggPlayerState = (int)OggPlayerState.PAUSED;

                OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline;
            }
        }

        private void OGGPlayerComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (oggPlayerState == (int)OggPlayerState.PLAYING && waveOut != null)
            {
                if (OGGPlayerTimer == null)
                {
                    return;
                }

                OGGPlayerTimer.Stop();

                waveOut.Stop();

                //WindowUtils.MessageBoxShow(waveStream.CurrentTime.ToString());

                OGGPlayer.Value = 0;

                pcmSource.Position = 0;

                oggPlayerPaused = false;

                oggPlayerStopped = true;

                OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline;

                string[] hashData = OGGPlayerLabelHashFileName.Content.ToString().Split('.');

                int return_value = RpkgLib.create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hashData[0], 1, OGGPlayerComboBox.SelectedIndex);

                string currentDirectory = Directory.GetCurrentDirectory();

                string inputOGGFile = currentDirectory + "\\" + hashData[0] + ".ogg";

                string outputPCMFile = currentDirectory + "\\" + hashData[0] + ".pcm";

                return_value = RpkgLib.convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

                if (return_value == 1)
                {
                    if (File.Exists(outputPCMFile))
                    {
                        if (OGGPlayerTimer != null)
                        {
                            OGGPlayerTimer.Stop();
                        }

                        if (waveOut != null)
                        {
                            waveOut.Stop();
                            waveOut.Dispose();
                            waveOut = null;
                        }

                        oggPlayerState = (int)OggPlayerState.NULL;

                        if (pcmMemoryStream != null)
                        {
                            pcmMemoryStream.Dispose();
                        }

                        pcmMemoryStream = new MemoryStream(File.ReadAllBytes(outputPCMFile));

                        int pcmSampleSize = RpkgLib.get_pcm_sample_size();
                        int pcmSampleRate = RpkgLib.get_pcm_sample_rate();
                        int pcmChannels = RpkgLib.get_pcm_channels();

                        File.Delete(hashData[0] + ".ogg");
                        File.Delete(hashData[0] + ".wem");
                        File.Delete(hashData[0] + ".pcm");

                        //file = new FileInfo("output.pcm");
                        //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                        waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                        pcmSource = new RawSourceWaveStream(pcmMemoryStream, waveFormat);

                        oggPlayerState = (int)OggPlayerState.RESET;
                    }
                }
                else
                {
                    if (TabAudioPlayer.Visibility == Visibility.Visible)
                    {
                        TabAudioPlayer.Visibility = Visibility.Collapsed;
                    }

                    TabDetails.IsSelected = true;
                }
            }
        }

        private void HashCalculator_Click(object sender, RoutedEventArgs e)
        {
            HashCalculator hashCalculator = new HashCalculator();

            SetDiscordStatus("Hash Calculator", "");

            hashCalculator.ShowDialog();

            if (LeftTabControl.SelectedIndex == 0)
            {
                SetDiscordStatus("Resource View", "");
            }
            else if (LeftTabControl.SelectedIndex == 1)
            {
                SetDiscordStatus("Dependency View", "");
            }
            else if (LeftTabControl.SelectedIndex == 2)
            {
                SetDiscordStatus("Search View", "");
            }
        }

        private void ConvertHashMetaFileToJSON_Click(object sender, RoutedEventArgs e)
        {
            var fileDialog = new VistaOpenFileDialog();

            fileDialog.Title = "Select an input Hash meta file:";

            fileDialog.Filter = "meta file|*.meta|All files|*.*";

            if (!Directory.Exists(App.Settings.InputFolder))
            {
                App.Settings.InputFolder = Directory.GetCurrentDirectory();

                var options = new JsonSerializerOptions { WriteIndented = true };

                string jsonString = JsonSerializer.Serialize(App.Settings, options);

                File.WriteAllText("rpkg.json", jsonString);
            }

            fileDialog.InitialDirectory = App.Settings.InputFolder;

            fileDialog.FileName = App.Settings.InputFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                if (fileDialog.FileName.EndsWith(".meta", StringComparison.OrdinalIgnoreCase))
                {
                    string command = "";
                    string input_path = fileDialog.FileName;
                    string filter = "";
                    string search = "";
                    string search_type = "";
                    string output_path = "";

                    Progress progress = new Progress();

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    command = "-hash_meta_to_json";

                    progress.message.Content = "Converting Hash meta to JSON...";

                    progress.ProgressBar.IsIndeterminate = true;

                    int return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    string responseString = Marshal.PtrToStringAnsi(RpkgLib.get_response_string());

                    if (responseString != "")
                    {
                        WindowUtils.MessageBoxShow(responseString);
                    }
                }
                else
                {
                    WindowUtils.MessageBoxShow("Error: The input file must end in .meta");
                }
            }
        }

        private void ConvertJSONToHashMetaFile_Click(object sender, RoutedEventArgs e)
        {
            var fileDialog = new VistaOpenFileDialog();

            fileDialog.Title = "Select an input Hash meta.JSON file:";

            fileDialog.Filter = "meta.JSON file|*.meta.JSON|All files|*.*";

            if (!Directory.Exists(App.Settings.InputFolder))
            {
                App.Settings.InputFolder = Directory.GetCurrentDirectory();

                var options = new JsonSerializerOptions { WriteIndented = true };

                string jsonString = JsonSerializer.Serialize(App.Settings, options);

                File.WriteAllText("rpkg.json", jsonString);
            }

            fileDialog.InitialDirectory = App.Settings.InputFolder;

            fileDialog.FileName = App.Settings.InputFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                if (fileDialog.FileName.EndsWith(".meta.JSON", StringComparison.OrdinalIgnoreCase))
                {
                    string command = "";
                    string input_path = fileDialog.FileName;
                    string filter = "";
                    string search = "";
                    string search_type = "";
                    string output_path = "";

                    Progress progress = new Progress();

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    command = "-json_to_hash_meta";

                    progress.message.Content = "Converting Hash meta.JSON to meta...";

                    progress.ProgressBar.IsIndeterminate = true;

                    int return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task rpkgExecute = RpkgLib.task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    string responseString = Marshal.PtrToStringAnsi(RpkgLib.get_response_string());

                    if (responseString != "")
                    {
                        WindowUtils.MessageBoxShow(responseString);
                    }
                }
                else
                {
                    WindowUtils.MessageBoxShow("Error: The input file must end in .meta.JSON");
                }
            }
        }
    }
}

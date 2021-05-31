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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using MahApps.Metro.Controls;
using ControlzEx.Theming;
using System.Text.Json;
using System.Text.Json.Serialization;
using Ookii.Dialogs.Wpf;
using System.Net.Http;
using System.Diagnostics;
using System.Security;
using NAudio.Wave;
using HelixToolkit.Wpf;
using HelixToolkit;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.CompilerServices;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        public MainWindow()
        {
            InitializeComponent();
            ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.DodgerBlue)));
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

                var options = new JsonSerializerOptions { WriteIndented = true };

                userSettings = new UserSettings();

                userSettings.ColorTheme = "Dark/Red";
                userSettings.InputFolder = System.IO.Directory.GetCurrentDirectory();
                userSettings.OutputFolder = System.IO.Directory.GetCurrentDirectory();

                string jsonString = JsonSerializer.Serialize(userSettings, options);

                File.WriteAllText("rpkg.json", jsonString);
            }
            else
            {
                string jsonString = File.ReadAllText("rpkg.json");

                userSettings = JsonSerializer.Deserialize<UserSettings>(jsonString);

                string[] theme = userSettings.ColorTheme.Split('/');

                string brightness = theme[0];
                string color = theme[1];

                SetColorTheme(brightness, color);
            }

            if (!File.Exists("hash_list.txt"))
            {
                MessageQuestion messageBox = new MessageQuestion();
                messageBox.message.Content = "Error: The hast list file (hash_list.txt) is missing.\n\nIt is necessary to have this file for the optimal functioning of this program.\n\nClick OK to have it downloaded and extracted manually.\n\nThe program will automatically restart when done.\n\nYou can also download it manually from https://hitmandb.notex.app/latest-hashes.7z and extract it to the same directory as this program.";
                messageBox.ShowDialog();

                //MessageBoxShow(messageBox.buttonPressed);

                if (messageBox.buttonPressed == "OKButton")
                {
                    DownloadExtractHashList();
                    System.Windows.Forms.Application.Restart();
                    Environment.Exit(0);
                }
                else if (messageBox.buttonPressed == "CancelButton")
                {
                    Environment.Exit(0);
                }
            }

            int currentVersion = load_hash_list();

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
                    Environment.Exit(0);
                }
            }

            EventManager.RegisterClassHandler(typeof(TreeViewItem), TreeViewItem.PreviewMouseLeftButtonUpEvent, new RoutedEventHandler(TreeViewItem_PreviewMouseLeftButtonUpEvent));
            EventManager.RegisterClassHandler(typeof(TreeViewItem), TreeViewItem.PreviewMouseRightButtonDownEvent, new RoutedEventHandler(TreeViewItem_PreviewMouseRightButtonDownEvent));
            EventManager.RegisterClassHandler(typeof(TreeViewItem), TreeViewItem.MouseRightButtonDownEvent, new RoutedEventHandler(TreeViewItem_MouseRightButtonDownEvent));

            var item = new TreeViewItem();
            item.Header = "Click";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "File->Import RPKG File";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Or";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "File->Import RPKGs Folder";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Above to start!";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "After any RPKGs have been imported you can left or right click on the RPKG";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "or on any item in the Resources or Dependency Views.";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Left clicking causes the Details/Hex Viewer/JSON Viewer on the right to update.";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Right clicking provides you with a popup menu for further options.";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Alternatively you can select from the:";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Extract";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Generate";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Mass Extract";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Rebuild";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "Color Theme";
            MainTreeView.Items.Add(item);
            item = new TreeViewItem();
            item.Header = "menus above for additional functions.";
            MainTreeView.Items.Add(item);

            MainTreeView.SelectedItemChanged += MainTreeView_SelectedItemChanged;

            HashMapTreeView.SelectedItemChanged += MainTreeView_SelectedItemChanged;

            SearchRPKGsTreeView.SelectedItemChanged += MainTreeView_SelectedItemChanged;

            SearchHashListTreeView.SelectedItemChanged += SearchHashListTreeView_SelectedItemChanged;

            LeftTabControl.SelectionChanged += LeftTabControl_SelectionChanged;

            RightTabControl.SelectionChanged += RightTabControl_SelectionChanged;
        }

        private void LeftTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            TabControl tab = (sender as TabControl);

            //MessageBoxShow(tab.SelectedIndex.ToString());

            if (tab.SelectedIndex == 1)
            {
                LoadHashDependsMap();
            }
        }

        private void RightTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            TabControl tab = (sender as TabControl);

            //MessageBoxShow(tab.SelectedIndex.ToString());

            if (tab.SelectedIndex == 1)
            {
                LoadHexEditor();
            }
        }

        private void LoadHexEditor()
        {
            HexViewerTextBox.Text = "Hex view of " + currentHash + ":\n\n";

            string[] hashDetails = currentHash.Split('.');

            string hash = hashDetails[0];

            HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(currentRPKGFilePath, hash));
        }

        private void SearchHashListTreeView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (oneOrMoreRPKGsHaveBeenImported)
            {
                if (MainTreeView.Items.Count > 0)
                {
                    var treeview = (sender as TreeView);

                    string itemHeader = "";

                    if (treeview.SelectedItem is TreeViewItem item)
                    {
                        itemHeader = (item.Header as string);
                    }
                    else if (treeview.SelectedItem == null)
                    {
                        return;
                    }
                    else
                    {
                        itemHeader = treeview.SelectedItem.ToString();
                    }

                    string[] header = itemHeader.Split(' ');

                    string hashFile = header[0];

                    string[] hashDetails = hashFile.Split('.');

                    string hash = hashDetails[0];
                    string resourceType = hashDetails[1];

                    string rpkgHashFilePathString = Marshal.PtrToStringAnsi(get_rpkg_file_paths_hash_is_in(hash));

                    if (rpkgHashFilePathString != "")
                    {
                        string[] rpkgHashFilePaths = rpkgHashFilePathString.Trim(',').Split(',');

                        DetailsTextBox.Text = "";

                        HexViewerTextBox.Text = "";

                        LocalizationTextBox.Text = "";

                        foreach (string rpkgHashFilePath in rpkgHashFilePaths)
                        {
                            //MessageBoxShow(rpkgHashFilePath);

                            string identifier = rpkgHashFilePath + hash;

                            DetailsTextBox.Text += hash + " in RPKG file " + rpkgHashFilePath + ":\n";

                            DetailsTextBox.Text += Marshal.PtrToStringAnsi(get_hash_details(rpkgHashFilePath, hash));

                            DetailsTextBox.Text += "\n\n";

                            //HexViewerTextBox.Text = "Hex view of " + header[0] + ":\n\n";

                            LocalizationTextBox.Text = "";

                            //HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgHashFilePath, hash));

                            if (RightTabControl.SelectedIndex == 1)
                            {
                                LoadHexEditor();
                            }

                            currentRPKGFilePath = rpkgHashFilePath;
                            currentHash = header[0];

                            if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                            {
                                UInt32 localization_data_size = generate_localization_string(rpkgHashFilePath, hash, resourceType);

                                byte[] localization_data = new byte[localization_data_size];

                                Marshal.Copy(get_localization_string(), localization_data, 0, (int)localization_data_size);

                                if (localization_data_size > 0)
                                {
                                    LocalizationTextBox.Text = Encoding.UTF8.GetString(localization_data);
                                }

                                if (localization_data_size > 0)
                                {
                                    if (ThirdTabRight.Visibility == Visibility.Collapsed)
                                    {
                                        ThirdTabRight.Visibility = Visibility.Visible;
                                    }
                                }
                                else
                                {
                                    if (ThirdTabRight.Visibility == Visibility.Visible)
                                    {
                                        ThirdTabRight.Visibility = Visibility.Collapsed;
                                    }
                                }
                            }
                            else
                            {
                                if (ThirdTabRight.Visibility == Visibility.Visible)
                                {
                                    ThirdTabRight.Visibility = Visibility.Collapsed;
                                }

                                if (FourthTabRight.Visibility == Visibility.Visible)
                                {
                                    FourthTabRight.Visibility = Visibility.Collapsed;
                                }

                                if (FifthTabRight.Visibility == Visibility.Visible)
                                {
                                    FifthTabRight.Visibility = Visibility.Collapsed;
                                }

                                if (SixthTabRight.Visibility == Visibility.Visible)
                                {
                                    SixthTabRight.Visibility = Visibility.Collapsed;
                                }
                            }

                            int return_value = clear_hash_data_vector();
                        }
                    }
                    else
                    {
                        DetailsTextBox.Text = "Hash is not located in any of the currently imported RPKGs.";

                        HexViewerTextBox.Text = "";

                        LocalizationTextBox.Text = "";
                    }

                    GC.Collect();
                }
            }
            else
            {
                DetailsTextBox.Text = "Hash is not located in any of the currently imported RPKGs.";

                HexViewerTextBox.Text = "";

                LocalizationTextBox.Text = "";
            }
        }

        public static void TEXTToPNGThread(string rpkgFilePath, string hash, string png_file_name)
        {
            int return_value = generate_png_from_text(rpkgFilePath, hash, png_file_name);
        }

        public static void MassExtractTEXTThread(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            int return_value = task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public int MassExtractTEXT(string command, string input_path, string filter, string search, string search_type, string output_path)
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
            int return_value = task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public int RebuildTEXT(string command, string input_path, string filter, string search, string search_type, string output_path)
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
            int return_value = task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public int ExtractTEXT(string command, string input_path, string filter, string search, string search_type, string output_path)
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
            int return_value = task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public int ExtractMODEL(string command, string input_path, string filter, string search, string search_type, string output_path)
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
            int return_value = task_execute(command, input_path, filter, search, search_type, output_path);
        }

        public int RebuildMODEL(string command, string input_path, string filter, string search, string search_type, string output_path)
        {
            Thread thread = new Thread(() => RebuildMODELThread(command, input_path, filter, search, search_type, output_path));
            thread.SetApartmentState(ApartmentState.MTA);
            thread.Start();
            thread.Join();
            thread = null;

            return 0;
        }

        private void MainTreeView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (oneOrMoreRPKGsHaveBeenImported)
            {
                if (MainTreeView.Items.Count > 0)
                {
                    var treeview = (sender as TreeView);

                    string itemHeader = "";

                    if (treeview.SelectedItem is TreeViewItem item)
                    {
                        itemHeader = (item.Header as string);
                    }
                    else if (treeview.SelectedItem == null)
                    {
                        return;
                    }
                    else
                    {
                        itemHeader = treeview.SelectedItem.ToString();
                    }

                    //MessageBoxShow(itemHeader);

                    if (itemHeader.Length == 4)
                    {
                        string resourceType = itemHeader;

                        string rpkgFile = itemHeader.Substring(itemHeader.LastIndexOf("\\") + 1);

                        UInt32 hashBasedOnResourceTypeCount = get_hash_based_on_resource_type_count(rpkgFilePath, resourceType);

                        UInt64 resourceTypesDataSize = get_resource_types_data_size(rpkgFilePath, resourceType);

                        DetailsTextBox.Text = rpkgFile + " has " + hashBasedOnResourceTypeCount.ToString("N0") + " hash files/resources of resource type " + resourceType + ",\n";
                        DetailsTextBox.Text += "having a combined total data size of " + resourceTypesDataSize.ToString("N0") + " bytes";

                        HexViewerTextBox.Text = "";

                        if (ThirdTabRight.Visibility == Visibility.Visible)
                        {
                            ThirdTabRight.Visibility = Visibility.Collapsed;
                        }

                        if (FourthTabRight.Visibility == Visibility.Visible)
                        {
                            FourthTabRight.Visibility = Visibility.Collapsed;
                        }

                        if (FifthTabRight.Visibility == Visibility.Visible)
                        {
                            FifthTabRight.Visibility = Visibility.Collapsed;
                        }

                        if (SixthTabRight.Visibility == Visibility.Visible)
                        {
                            SixthTabRight.Visibility = Visibility.Collapsed;
                        }

                        DetailsTextBox.Visibility = Visibility.Visible;
                        MainScrollViewer.Visibility = Visibility.Collapsed;
                        MainStackPanel.Visibility = Visibility.Collapsed;
                        EditPatchDeletionListButton.Visibility = Visibility.Collapsed;
                        SaveChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                        DiscardChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                        EditHashDependsButton.Visibility = Visibility.Collapsed;
                        SaveChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                        DiscardChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                        ExtractHashDependsButton.Visibility = Visibility.Collapsed;
                    }
                    else if (itemHeader.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                    {
                        string rpkgFile = itemHeader.Substring(itemHeader.LastIndexOf("\\") + 1);

                        UInt32 allHashesInRPKGCount = get_all_hashes_in_rpkg_count(itemHeader);

                        UInt64 allHashesInRPKGDataSize = get_all_hashes_in_rpkg_data_size(itemHeader);

                        DetailsTextBox.Text = rpkgFile + " has " + allHashesInRPKGCount.ToString("N0") + " hash files/resources,\n";
                        DetailsTextBox.Text += "having a combined total data size of " + allHashesInRPKGDataSize.ToString("N0") + " bytes";

                        string patchDataString = Marshal.PtrToStringAnsi(get_patch_deletion_list(itemHeader));

                        if (patchDataString != "")
                        {
                            DetailsTextBox.Text += patchDataString;

                            DetailsTextBox.Visibility = Visibility.Visible;
                            MainScrollViewer.Visibility = Visibility.Collapsed;
                            MainStackPanel.Visibility = Visibility.Collapsed;
                            EditPatchDeletionListButton.Visibility = Visibility.Visible;
                            SaveChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                            DiscardChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                            EditHashDependsButton.Visibility = Visibility.Collapsed;
                            SaveChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                            DiscardChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                            ExtractHashDependsButton.Visibility = Visibility.Collapsed;
                        }
                        else
                        {
                            DetailsTextBox.Visibility = Visibility.Visible;
                            MainScrollViewer.Visibility = Visibility.Collapsed;
                            MainStackPanel.Visibility = Visibility.Collapsed;
                            EditPatchDeletionListButton.Visibility = Visibility.Collapsed;
                            SaveChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                            DiscardChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                            EditHashDependsButton.Visibility = Visibility.Collapsed;
                            SaveChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                            DiscardChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                            ExtractHashDependsButton.Visibility = Visibility.Collapsed;
                        }

                        HexViewerTextBox.Text = "";

                        if (ThirdTabRight.Visibility == Visibility.Visible)
                        {
                            ThirdTabRight.Visibility = Visibility.Collapsed;
                        }

                        if (FourthTabRight.Visibility == Visibility.Visible)
                        {
                            FourthTabRight.Visibility = Visibility.Collapsed;
                        }

                        if (FifthTabRight.Visibility == Visibility.Visible)
                        {
                            FifthTabRight.Visibility = Visibility.Collapsed;
                        }

                        if (SixthTabRight.Visibility == Visibility.Visible)
                        {
                            SixthTabRight.Visibility = Visibility.Collapsed;
                        }
                    }
                    else
                    {
                        string[] header = itemHeader.Split(' ');

                        string hashFile = header[0];
                        string ioiString = header[1];

                        string[] hashDetails = hashFile.Split('.');

                        string hash = hashDetails[0];
                        string resourceType = hashDetails[1];

                        currentHashFileName = hashFile;

                        DetailsTextBox.Text = hash + " in RPKG file " + rpkgFilePath + ":\n";

                        DetailsTextBox.Text += Marshal.PtrToStringAnsi(get_hash_details(rpkgFilePath, hash));

                        hashDependsRPKGFilePath = rpkgFilePath;

                        DetailsTextBox.Visibility = Visibility.Visible;
                        MainScrollViewer.Visibility = Visibility.Collapsed;
                        MainStackPanel.Visibility = Visibility.Collapsed;
                        EditPatchDeletionListButton.Visibility = Visibility.Collapsed;
                        SaveChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                        DiscardChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
                        EditHashDependsButton.Visibility = Visibility.Visible;
                        SaveChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                        DiscardChangesToHashDependsButton.Visibility = Visibility.Collapsed;
                        ExtractHashDependsButton.Visibility = Visibility.Visible;

                        //HexViewerTextBox.Text = "Hex view of " + header[0] + ":\n\n";
                        //HexViewerTextBoxTextString = "Hex view of " + header[0] + ":\n\n";

                        LocalizationTextBox.Text = "";

                        //HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash));
                        //HexViewerTextBoxTextString += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash));

                        if (RightTabControl.SelectedIndex == 1)
                        {
                            LoadHexEditor();
                        }

                        currentRPKGFilePath = rpkgFilePath;
                        currentHash = header[0];

                        if (resourceType == "JSON")
                        {
                            UInt32 json_data_size = generate_json_string(rpkgFilePath, hash);

                            byte[] json_data = new byte[json_data_size];

                            Marshal.Copy(get_json_string(), json_data, 0, (int)json_data_size);

                            if (json_data_size > 0)
                            {
                                LocalizationTextBox.Text = Encoding.UTF8.GetString(json_data);
                            }

                            if (json_data_size > 0)
                            {
                                if (ThirdTabRight.Visibility == Visibility.Collapsed)
                                {
                                    ThirdTabRight.Visibility = Visibility.Visible;
                                }
                            }
                            else
                            {
                                if (ThirdTabRight.Visibility == Visibility.Visible)
                                {
                                    ThirdTabRight.Visibility = Visibility.Collapsed;
                                }
                            }
                        }
                        else if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                        {
                            UInt32 localization_data_size = generate_localization_string(rpkgFilePath, hash, resourceType);

                            byte[] localization_data = new byte[localization_data_size];

                            Marshal.Copy(get_localization_string(), localization_data, 0, (int)localization_data_size);

                            if (localization_data_size > 0)
                            {
                                LocalizationTextBox.Text = Encoding.UTF8.GetString(localization_data);
                            }

                            if (localization_data_size > 0)
                            {
                                if (ThirdTabRight.Visibility == Visibility.Collapsed)
                                {
                                    ThirdTabRight.Visibility = Visibility.Visible;
                                }
                            }
                            else
                            {
                                if (ThirdTabRight.Visibility == Visibility.Visible)
                                {
                                    ThirdTabRight.Visibility = Visibility.Collapsed;
                                }
                            }
                        }
                        else
                        {
                            if (ThirdTabRight.Visibility == Visibility.Visible)
                            {
                                ThirdTabRight.Visibility = Visibility.Collapsed;
                            }

                            if (FourthTabRight.Visibility == Visibility.Visible)
                            {
                                FourthTabRight.Visibility = Visibility.Collapsed;
                            }

                            if (FifthTabRight.Visibility == Visibility.Visible)
                            {
                                FifthTabRight.Visibility = Visibility.Collapsed;
                            }

                            if (SixthTabRight.Visibility == Visibility.Visible)
                            {
                                SixthTabRight.Visibility = Visibility.Collapsed;
                            }
                        }

                        int return_value = clear_hash_data_vector();

                        if (resourceType == "PRIM")
                        {
                            string command = "-extract_prim_to_obj_from";
                            string input_path = rpkgFilePath;
                            string filter = hash;
                            string search = "";
                            string search_type = "";
                            string output_path = "";

                            return_value = reset_task_status();

                            return_value = task_execute(command, input_path, filter, search, search_type, output_path);

                            string currentDirectory = System.IO.Directory.GetCurrentDirectory();

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
                                if (SixthTabRight.Visibility == Visibility.Collapsed)
                                {
                                    SixthTabRight.Visibility = Visibility.Visible;
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
                                CameraHelper.ZoomExtents(helixViewport.Camera, helixViewport.Viewport, 1000);

                                foreach (string filePath in objFileNames)
                                {
                                    File.Delete(filePath);
                                }
                            }
                            else
                            {
                                if (SixthTabRight.Visibility == Visibility.Visible)
                                {
                                    SixthTabRight.Visibility = Visibility.Collapsed;
                                }
                            }
                        }

                        if (resourceType == "GFXI")
                        {
                            BitmapImage bitmapImage = new BitmapImage();

                            bitmapImage.BeginInit();

                            UInt32 hash_size = get_hash_in_rpkg_size(rpkgFilePath, hash);

                            byte[] hash_data = new byte[hash_size];

                            Marshal.Copy(get_hash_in_rpkg_data(rpkgFilePath, hash), hash_data, 0, (int)hash_size);

                            MemoryStream memoryStream = new MemoryStream(hash_data);

                            bitmapImage.StreamSource = memoryStream;

                            bitmapImage.EndInit();

                            ImageViewer.Source = bitmapImage;

                            if (hash_size > 0)
                            {
                                if (FourthTabRight.Visibility == Visibility.Collapsed)
                                {
                                    FourthTabRight.Visibility = Visibility.Visible;
                                }
                            }
                            else
                            {
                                if (FourthTabRight.Visibility == Visibility.Visible)
                                {
                                    FourthTabRight.Visibility = Visibility.Collapsed;
                                }
                            }

                            int return_value_clear = clear_hash_data_vector();
                        }

                        if (resourceType == "TEXT")
                        {
                            BitmapImage bitmapImage = new BitmapImage();

                            bitmapImage.BeginInit();

                            string currentDirectory = System.IO.Directory.GetCurrentDirectory();

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

                                if (FourthTabRight.Visibility == Visibility.Collapsed)
                                {
                                    FourthTabRight.Visibility = Visibility.Visible;
                                }

                                File.Delete(png_file_name);
                            }
                            else
                            {
                                if (FourthTabRight.Visibility == Visibility.Visible)
                                {
                                    FourthTabRight.Visibility = Visibility.Collapsed;
                                }
                            }

                            int return_value_clear = clear_hash_data_vector();
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

                            if (FifthTabRight.Visibility == Visibility.Collapsed)
                            {
                                FifthTabRight.Visibility = Visibility.Visible;
                            }

                            return_value = create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hash, 0, 0);

                            string currentDirectory = System.IO.Directory.GetCurrentDirectory();

                            string inputOGGFile = currentDirectory + "\\" + hash + ".ogg";

                            string outputPCMFile = currentDirectory + "\\" + hash + ".pcm";

                            return_value = convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

                            if (File.Exists(outputPCMFile))
                            {
                                if (pcmMemoryStream != null)
                                {
                                    pcmMemoryStream.Dispose();
                                }

                                pcmMemoryStream = new MemoryStream(File.ReadAllBytes(outputPCMFile));

                                int pcmSampleSize = get_pcm_sample_size();
                                int pcmSampleRate = get_pcm_sample_rate();
                                int pcmChannels = get_pcm_channels();

                                File.Delete(hash + ".ogg");
                                File.Delete(hash + ".wem");
                                File.Delete(hash + ".pcm");

                                //file = new FileInfo("output.pcm");
                                //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                                waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                                pcmSource = new NAudio.Wave.RawSourceWaveStream(pcmMemoryStream, waveFormat);

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

                            int wwevCount = create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hash, 0, 0);

                            if (wwevCount > 0)
                            {
                                if (FifthTabRight.Visibility == Visibility.Collapsed)
                                {
                                    FifthTabRight.Visibility = Visibility.Visible;
                                }

                                return_value = create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hash, 1, 0);

                                string currentDirectory = System.IO.Directory.GetCurrentDirectory();

                                string inputOGGFile = currentDirectory + "\\" + hash + ".ogg";

                                string outputPCMFile = currentDirectory + "\\" + hash + ".pcm";

                                return_value = convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

                                if (return_value == 1)
                                {
                                    if (File.Exists(outputPCMFile))
                                    {
                                        if (pcmMemoryStream != null)
                                        {
                                            pcmMemoryStream.Dispose();
                                        }

                                        pcmMemoryStream = new MemoryStream(File.ReadAllBytes(outputPCMFile));

                                        int pcmSampleSize = get_pcm_sample_size();
                                        int pcmSampleRate = get_pcm_sample_rate();
                                        int pcmChannels = get_pcm_channels();

                                        File.Delete(hash + ".ogg");
                                        File.Delete(hash + ".wem");
                                        File.Delete(hash + ".pcm");

                                        //file = new FileInfo("output.pcm");
                                        //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                                        waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                                        pcmSource = new NAudio.Wave.RawSourceWaveStream(pcmMemoryStream, waveFormat);

                                        oggPlayerState = (int)OggPlayerState.READY;
                                    }

                                    for (int i = 0; i < wwevCount; i++)
                                    {
                                        OGGPlayerComboBox.Items.Add(i.ToString() + ".ogg");
                                    }

                                    OGGPlayerComboBox.SelectedIndex = 0;
                                }
                                else
                                {
                                    if (FifthTabRight.Visibility == Visibility.Visible)
                                    {
                                        FifthTabRight.Visibility = Visibility.Collapsed;
                                    }

                                    FirstTabRight.IsSelected = true;
                                }
                            }
                            else
                            {
                                if (FifthTabRight.Visibility == Visibility.Visible)
                                {
                                    FifthTabRight.Visibility = Visibility.Collapsed;
                                }

                                FirstTabRight.IsSelected = true;
                            }
                        }
                    }

                    GC.Collect();
                }
            }
        }

        private void Item2_GotFocus(object sender, RoutedEventArgs e)
        {
            string[] header = ((sender as ListViewItem).Content as string).Split(' ');

            string hash = header[0];

            DetailsTextBox.Text = Marshal.PtrToStringAnsi(get_hash_details(rpkgFilePath, hash));

            UInt32 hash_size = get_hash_in_rpkg_size(rpkgFilePath, hash);

            byte[] hash_data = new byte[hash_size];

            Marshal.Copy(get_hash_in_rpkg_data(rpkgFilePath, hash), hash_data, 0, (int)hash_size);

            int return_value = clear_hash_data_vector();
        }

        private void ImportRPKGFile(string rpkgPath)
        {
            rpkgFilePath = rpkgPath;

            string command = "-import_rpkg";
            string input_path = rpkgFilePath;
            string filter = "";
            string search = "";
            string search_type = "";
            string output_path = "";

            string rpkgFile = rpkgFilePath.Substring(rpkgFilePath.LastIndexOf("\\") + 1);

            foreach (TreeViewItem treeviewItem in MainTreeView.Items)
            {
                if (rpkgFilePath == (treeviewItem.Header as string))
                {
                    return;
                }
            }

            int return_value = reset_task_status();

            execute_task rpkgExecute = task_execute;

            IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

            Progress progress = new Progress();

            progress.message.Content = "Importing RPKG file " + rpkgFile + "...";

            progress.operation = (int)Progress.Operation.IMPORT;

            progress.ShowDialog();

            if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
            {
                //MessageBoxShow(progress.task_status_string);

                return;
            }

            if (MainTreeView.Items.Count > 0)
            {
                if ((MainTreeView.Items.GetItemAt(0) as TreeViewItem).Header.ToString() == "Click")
                {
                    MainTreeView.Items.Clear();
                }
            }

            var item = new TreeViewItem();

            item.Header = rpkgFilePath;

            MainTreeView.Items.Add(item);

            Thread.Sleep(200);

            List<string> resourceTypes = new List<string>();

            UInt32 resourceTypeCount = get_resource_types_count(rpkgFilePath);

            for (UInt32 i = 0; i < resourceTypeCount; i++)
            {
                resourceTypes.Add(Marshal.PtrToStringAnsi(get_resource_types_at(rpkgFilePath, i)));
            }

            resourceTypes.Sort();

            foreach (string resourceType in resourceTypes)
            {
                var item2 = new TreeViewItem();

                item2.Header = resourceType;

                item2.Items.Add(null);

                item2.Expanded += Item2_Expanded;

                //item2.Collapsed += Item2_Collapsed;

                item.Items.Add(item2);
            }

            oneOrMoreRPKGsHaveBeenImported = true;
        }

        private void OpenRPKGFile_Click(object sender, RoutedEventArgs e)
        {
            var fileDialog = new Ookii.Dialogs.Wpf.VistaOpenFileDialog();

            fileDialog.Title = "Select an RPKG file to import:";

            fileDialog.Filter = "RPKG file|*.rpkg|All files|*.*";

            if (!System.IO.Directory.Exists(userSettings.InputFolder))
            {
                userSettings.InputFolder = System.IO.Directory.GetCurrentDirectory();

                var options = new JsonSerializerOptions { WriteIndented = true };

                string jsonString = JsonSerializer.Serialize(userSettings, options);

                File.WriteAllText("rpkg.json", jsonString);
            }

            fileDialog.InitialDirectory = userSettings.InputFolder;

            fileDialog.FileName = userSettings.InputFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                ImportRPKGFile(fileDialog.FileName);

                //LoadHashDependsMap();
            }
        }

        private void Item2_Expanded(object sender, RoutedEventArgs e)
        {
            var item = (sender as TreeViewItem);

            if (item.Items.Count == 1 && item.Items[0] == null)
            {
                item.Items.Clear();

                string[] header = (item.Header as string).Split(' ');

                string resourceType = header[0];

                UInt32 hashBasedOnResourceTypeCount = get_hash_based_on_resource_type_count(rpkgFilePath, resourceType);

                for (UInt32 i = 0; i < hashBasedOnResourceTypeCount; i++)
                {
                    string hash = "";

                    hash = Marshal.PtrToStringAnsi(get_hash_based_on_resource_type_at(rpkgFilePath, resourceType, i));

                    string[] temp_test = hash.Split('.');

                    string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

                    item.Items.Add(hash + " " + ioiString);
                }
            }

            GC.Collect();
        }

        private void TreeViewItem_PreviewMouseLeftButtonUpEvent(object sender, RoutedEventArgs e)
        {
            (sender as TreeViewItem).IsSelected = true;

            string header = (sender as TreeViewItem).Header.ToString();

            if (header.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
            {
                rpkgFilePath = header;
            }
        }

        private void TreeViewItem_PreviewMouseRightButtonDownEvent(object sender, RoutedEventArgs e)
        {
            (sender as TreeViewItem).IsSelected = true;

            string header = (sender as TreeViewItem).Header.ToString();

            if (header.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
            {
                rpkgFilePath = header;
            }
        }

        private void TreeViewItem_MouseRightButtonDownEvent(object sender, RoutedEventArgs e)
        {
            (sender as TreeViewItem).IsSelected = true;

            string header = (sender as TreeViewItem).Header.ToString();

            if (header.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
            {
                if (rightClickedOn == 0)
                {
                    string command = "";
                    string input_path = rpkgFilePath;
                    string filter = "";
                    string search = "";
                    string search_type = "";
                    string output_path = userSettings.OutputFolder;

                    Progress progress = new Progress();

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    string rpkgFile = rpkgFilePath.Substring(rpkgFilePath.LastIndexOf("\\") + 1);

                    string[] buttons = { "Extract All", "Unload " + rpkgFile + " from RPKG", "Cancel" };

                    RightClickMenu rightClickMenu = new RightClickMenu(buttons);

                    rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                    rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                    rightClickMenu.ShowDialog();

                    //MessageBoxShow(rightClickMenu.buttonPressed);
                    //MessageBoxShow(rpkgFilePath);

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
                        int temp_return_value = unload_rpkg(rpkgFilePath);

                        MainTreeView.Items.Remove((sender as TreeViewItem));

                        SearchRPKGsTreeView.Items.Clear();

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;

                        return;
                    }
                    else
                    {
                        rightClickedOn = 0;

                        rightClickedOnFirst = true;

                        return;
                    }

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;

                        return;
                    }

                    rightClickedOn = 0;

                    rightClickedOnFirst = true;
                }
                else if (rightClickedOn == 1)
                {
                    string command = "";
                    string input_path = rpkgFilePath;
                    string filter = "";
                    string search = "";
                    string search_type = "";
                    string output_path = userSettings.OutputFolder;

                    Progress progress = new Progress();

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    if (rightClickedOnName == "PRIM")
                    {
                        string[] buttons = { "Extract All " + rightClickedOnName, "Extract All " + rightClickedOnName + " To GLB", "Cancel" };

                        RightClickMenu rightClickMenu = new RightClickMenu(buttons);

                        rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                        rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                        rightClickMenu.ShowDialog();

                        //MessageBoxShow(rightClickMenu.buttonPressed);
                        //MessageBoxShow(rpkgFilePath);

                        if (rightClickMenu.buttonPressed == "button0")
                        {
                            command = "-extract_from_rpkg";

                            filter = rightClickedOnName;

                            progress.message.Content = "Extracting All " + rightClickedOnName + "...";
                        }
                        else if (rightClickMenu.buttonPressed == "button1")
                        {
                            command = "-extract_all_prim_to_glb_from";

                            progress.message.Content = "Extracting All " + rightClickedOnName + " To GLB...";

                            string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                            if (temp_outputFolder == "")
                            {
                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            output_path = temp_outputFolder;
                        }
                        else
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;
                    }
                    else if (rightClickedOnName == "TEXT")
                    {
                        string[] buttons = { "Extract All " + rightClickedOnName, "Extract All " + rightClickedOnName + " To TGA", "Cancel" };

                        RightClickMenu rightClickMenu = new RightClickMenu(buttons);

                        rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                        rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                        rightClickMenu.ShowDialog();

                        //MessageBoxShow(rightClickMenu.buttonPressed);
                        //MessageBoxShow(rpkgFilePath);

                        if (rightClickMenu.buttonPressed == "button0")
                        {
                            command = "-extract_from_rpkg";

                            filter = rightClickedOnName;

                            progress.message.Content = "Extracting All " + rightClickedOnName + "...";
                        }
                        else if (rightClickMenu.buttonPressed == "button1")
                        {
                            command = "-extract_all_text_from";

                            progress.message.Content = "Extracting All " + rightClickedOnName + " To TGA...";

                            string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                            if (temp_outputFolder == "")
                            {
                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            output_path = temp_outputFolder;
                        }
                        else
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;
                    }
                    else if (rightClickedOnName == "LOCR" || rightClickedOnName == "DLGE" || rightClickedOnName == "RTLV")
                    {
                        string[] buttons = { "Extract All " + rightClickedOnName, "Extract All " + rightClickedOnName + " To JSON", "Cancel" };

                        RightClickMenu rightClickMenu = new RightClickMenu(buttons);

                        rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                        rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                        rightClickMenu.ShowDialog();

                        //MessageBoxShow(rightClickMenu.buttonPressed);
                        //MessageBoxShow(rpkgFilePath);

                        if (rightClickMenu.buttonPressed == "button0")
                        {
                            command = "-extract_from_rpkg";

                            filter = rightClickedOnName;

                            progress.message.Content = "Extracting All " + rightClickedOnName + "...";
                        }
                        else if (rightClickMenu.buttonPressed == "button1")
                        {
                            if (rightClickedOnName == "LOCR")
                            {
                                command = "-extract_locr_to_json_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To JSON...";
                            }
                            else if (rightClickedOnName == "DLGE")
                            {
                                command = "-extract_dlge_to_json_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To JSON...";
                            }
                            else if (rightClickedOnName == "RTLV")
                            {
                                command = "-extract_rtlv_to_json_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To JSON...";
                            }
                        }
                        else
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;
                    }
                    else if (rightClickedOnName == "WWEM" || rightClickedOnName == "WWES" || rightClickedOnName == "WWEV" || rightClickedOnName == "ORES" || rightClickedOnName == "GFXI" || rightClickedOnName == "JSON")
                    {
                        string buttonContent = "";

                        if (rightClickedOnName == "WWEM" || rightClickedOnName == "WWES" || rightClickedOnName == "WWEV")
                        {
                            buttonContent = "Extract All " + rightClickedOnName + " To OGG To IOI Paths";
                        }
                        else if (rightClickedOnName == "ORES")
                        {
                            buttonContent = "Extract All " + rightClickedOnName + " To IOI Paths";
                        }
                        else if (rightClickedOnName == "GFXI")
                        {
                            buttonContent = "Extract All " + rightClickedOnName + " To Images To IOI Paths";
                        }
                        else if (rightClickedOnName == "JSON")
                        {
                            buttonContent = "Extract All " + rightClickedOnName + " To JSONs To IOI Paths";
                        }

                        string[] buttons = { "Extract All " + rightClickedOnName, buttonContent, "Cancel" };

                        RightClickMenu rightClickMenu = new RightClickMenu(buttons);

                        rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                        rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                        rightClickMenu.ShowDialog();

                        //MessageBoxShow(rightClickMenu.buttonPressed);
                        //MessageBoxShow(rpkgFilePath);

                        if (rightClickMenu.buttonPressed == "button0")
                        {
                            progress.ProgressBar.IsIndeterminate = true;

                            command = "-extract_from_rpkg";

                            filter = rightClickedOnName;

                            progress.message.Content = "Extracting All " + rightClickedOnName + "...";
                        }
                        else if (rightClickMenu.buttonPressed == "button1")
                        {
                            if (rightClickedOnName == "WWEM" || rightClickedOnName == "WWES" || rightClickedOnName == "WWEV" || rightClickedOnName == "ORES")
                            {
                                Filter filterDialog = new Filter();

                                filterDialog.message1.Content = "Enter extraction filter for " + rightClickedOnName + " below.";

                                filterDialog.ShowDialog();

                                filter = filterDialog.filterString;
                            }

                            if (rightClickedOnName == "WWEM")
                            {
                                command = "-extract_wwem_to_ogg_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To OGG To IOI Paths...";
                            }
                            else if (rightClickedOnName == "WWES")
                            {
                                command = "-extract_wwes_to_ogg_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To OGG To IOI Paths...";
                            }
                            else if (rightClickedOnName == "WWEV")
                            {
                                command = "-extract_wwev_to_ogg_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To OGG To IOI Paths...";
                            }
                            else if (rightClickedOnName == "ORES")
                            {
                                command = "-extract_ores_from";

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To IOI Paths...";
                            }
                            else if (rightClickedOnName == "GFXI")
                            {
                                command = "-extract_ores_from";

                                filter = rightClickedOnName;

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To IOI Paths...";
                            }
                            else if (rightClickedOnName == "JSON")
                            {
                                command = "-extract_ores_from";

                                filter = rightClickedOnName;

                                progress.message.Content = "Extracting All " + rightClickedOnName + " To IOI Paths...";
                            }
                        }
                        else
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;
                    }
                    else
                    {
                        string[] buttons = { "Extract All " + rightClickedOnName, "Cancel" };

                        RightClickMenu rightClickMenu = new RightClickMenu(buttons);

                        rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                        rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                        rightClickMenu.ShowDialog();

                        //MessageBoxShow(rightClickMenu.buttonPressed);
                        //MessageBoxShow(rpkgFilePath);

                        if (rightClickMenu.buttonPressed == "button0")
                        {
                            command = "-extract_from_rpkg";

                            filter = rightClickedOnName;

                            progress.message.Content = "Extracting " + rightClickedOnName + "...";
                        }
                        else
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;
                    }

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;

                        return;
                    }

                    rightClickedOn = 0;

                    rightClickedOnFirst = true;
                }
                else
                {
                    string command = "";
                    string input_path = rpkgFilePath;
                    string filter = "";
                    string search = "";
                    string search_type = "";
                    string output_path = userSettings.OutputFolder;

                    Progress progress = new Progress();

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    string[] hashData = rightClickedOnName.Split('.');

                    string hashValue = hashData[0];
                    string hashType = hashData[1];

                    RightClickMenu rightClickMenu;

                    int buttonCount = 0;

                    if (hashType == "PRIM")
                    {
                        string[] buttons = { "Extract " + rightClickedOnName, "Extract " + rightClickedOnName + " MODEL To GLB/TGA File(s)", "Extract " + rightClickedOnName + " To GLB File", "Cancel" };

                        buttonCount = 4;

                        rightClickMenu = new RightClickMenu(buttons);
                    }
                    else if (hashType == "TEMP")
                    {
                        string[] buttons = { "Extract " + rightClickedOnName, "Edit " + rightClickedOnName + " in Brick/Entity Editor", "Extract PRIM Models linked to " + rightClickedOnName + " To GLB/TGA File(s)", "Extract PRIMs linked to " + rightClickedOnName + " To GLB File(s)", "Cancel" };

                        buttonCount = 5;

                        rightClickMenu = new RightClickMenu(buttons);
                    }
                    else if (hashType == "TEXT")
                    {
                        string[] buttons = { "Extract " + rightClickedOnName, "Extract TEXTs(TEXDs) linked to " + rightClickedOnName + " To TGA File(s)", "Cancel" };

                        buttonCount = 3;

                        rightClickMenu = new RightClickMenu(buttons);
                    }
                    else if (hashType == "WWEM" || hashType == "WWES" || hashType == "WWEV")
                    {
                        string[] buttons = { "Extract " + rightClickedOnName, "Extract " + rightClickedOnName + " To OGG (IOI Path)", "Cancel" };

                        buttonCount = 3;

                        rightClickMenu = new RightClickMenu(buttons);
                    }
                    else if (hashType == "DLGE" || hashType == "LOCR" || hashType == "RTLV")
                    {
                        string[] buttons = { "Extract " + rightClickedOnName, "Extract " + rightClickedOnName + " To JSON (IOI Path)", "Cancel" };

                        buttonCount = 3;

                        rightClickMenu = new RightClickMenu(buttons);
                    }
                    else if (hashType == "GFXI")
                    {
                        if (rightClickedOnFullName.Contains("[ores:"))
                        {
                            string[] buttons = { "Extract " + rightClickedOnName, "Extract " + rightClickedOnName + " To Image (IOI Path)", "Cancel" };

                            buttonCount = 3;

                            rightClickMenu = new RightClickMenu(buttons);
                        }
                        else
                        {
                            string[] buttons = { "Extract " + rightClickedOnName, "Cancel" };

                            buttonCount = 2;

                            rightClickMenu = new RightClickMenu(buttons);
                        }
                    }
                    else if (hashType == "JSON")
                    {
                        if (rightClickedOnFullName.Contains("[ores:"))
                        {
                            string[] buttons = { "Extract " + rightClickedOnName, "Extract " + rightClickedOnName + " To JSON (IOI Path)", "Cancel" };

                            buttonCount = 3;

                            rightClickMenu = new RightClickMenu(buttons);
                        }
                        else
                        {
                            string[] buttons = { "Extract " + rightClickedOnName, "Cancel" };

                            buttonCount = 2;

                            rightClickMenu = new RightClickMenu(buttons);
                        }
                    }
                    else
                    {
                        string[] buttons = { "Extract " + rightClickedOnName, "Cancel" };

                        buttonCount = 2;

                        rightClickMenu = new RightClickMenu(buttons);
                    }

                    rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
                    rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

                    rightClickMenu.ShowDialog();

                    if (rightClickMenu.buttonPressed == "button0")
                    {
                        progress.ProgressBar.IsIndeterminate = true;

                        command = "-extract_from_rpkg";

                        filter = hashValue;

                        progress.message.Content = "Extracting " + rightClickedOnName + "...";

                        string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                        if (outputFolder == "")
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        output_path = outputFolder;
                    }
                    else if (rightClickMenu.buttonPressed == "button1" && buttonCount >= 3)
                    {
                        if (hashType == "PRIM")
                        {
                            string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

                            if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                            {
                                MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform PRIM model extraction.");

                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            List<string> rpkgFiles = new List<string>();

                            foreach (var filePath in Directory.GetFiles(runtimeDirectory))
                            {
                                if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                                {
                                    rpkgFiles.Add(filePath);
                                }
                            }

                            rpkgFiles.Sort(new NaturalStringComparer());

                            foreach (string filePath in rpkgFiles)
                            {
                                ImportRPKGFile(filePath);
                            }

                            command = "-extract_prim_model_from";

                            progress.operation = (int)Progress.Operation.GENERAL;

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To GLB/TGA File(s)...";

                            string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                            if (temp_outputFolder == "")
                            {
                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            output_path = temp_outputFolder;

                            int temp_return_value = reset_task_status();

                            execute_task temp_rpkgExecute = ExtractMODEL;

                            IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                            progress.ShowDialog();

                            if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                            {
                                //MessageBoxShow(progress.task_status_string);

                                return;
                            }

                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }
                        else if (hashType == "TEXT")
                        {
                            command = "-extract_text_from";

                            progress.operation = (int)Progress.Operation.GENERAL;

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To TGA File(s)...";

                            string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                            if (temp_outputFolder == "")
                            {
                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            output_path = temp_outputFolder;

                            int temp_return_value = reset_task_status();

                            execute_task temp_rpkgExecute = ExtractTEXT;

                            IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                            progress.ShowDialog();

                            if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                            {
                                //MessageBoxShow(progress.task_status_string);

                                return;
                            }

                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }
                        else if (hashType == "TEMP")
                        {
                            string rpkgFileBackup = rpkgFilePath;

                            string rpkgFile = rpkgFilePath.Substring(rpkgFilePath.LastIndexOf("\\") + 1);

                            string rpkgUpperName = rpkgFile.ToUpper();

                            if (rpkgUpperName.Contains("PATCH"))
                            {
                                string baseFileName = rpkgFile.Substring(0, rpkgUpperName.IndexOf("PATCH")).ToUpper();

                                string folderPath = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\") + 1);

                                List<string> rpkgFiles = new List<string>();

                                foreach (var filePath in Directory.GetFiles(folderPath))
                                {
                                    if (filePath.ToUpper().EndsWith(".RPKG"))
                                    {
                                        if (filePath.ToUpper().Contains("\\" + baseFileName.ToUpper() + ".RPKG"))
                                        {
                                            rpkgFiles.Add(filePath);
                                        }
                                        else if (filePath.ToUpper().Contains("\\" + baseFileName.ToUpper() + "PATCH"))
                                        {
                                            rpkgFiles.Add(filePath);
                                        }
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
                                }
                            }

                            int temp_return_value = clear_temp_tblu_data();

                            //MessageBoxShow(rightClickedOnName + ", " + rpkgFilePath);

                            rpkgFilePath = rpkgFileBackup;

                            temp_return_value = reset_task_status();

                            execute_load_recursive_temps load_recursive_temps_execute = load_recursive_temps;

                            IAsyncResult temp_ar = load_recursive_temps_execute.BeginInvoke(rightClickedOnName, rpkgFilePath, null, null);

                            Progress temp_progress = new Progress();

                            temp_progress.message.Content = "Analyzing Entity/Brick (TEMP/TBLU)...";

                            temp_progress.operation = (int)Progress.Operation.TEMP_TBLU;

                            temp_progress.ShowDialog();

                            int temp_index = get_temp_index(rightClickedOnName);

                            //MessageBoxShow(temp_index.ToString());

                            if (temp_progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                            {
                                if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_DEPENDS)
                                {
                                    MessageBoxShow("Error: " + rightClickedOnName + " file has no TBLU hash depends.");
                                }
                                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_RPKG)
                                {
                                    if (rpkgUpperName.Contains("PATCH"))
                                    {
                                        string rpkgBaseName = rpkgFile.Substring(0, rpkgUpperName.IndexOf("PATCH")) + ".rpkg";

                                        MessageBoxShow("Error: TBLU file linked to " + rightClickedOnName + " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.\n\nTry importing " + rpkgBaseName + " and trying to edit " + rightClickedOnName + " again.\n\nThis should be done before trying to launch the Brick/Entity Editor.");
                                    }
                                    else
                                    {
                                        MessageBoxShow("Error: TBLU file linked to " + rightClickedOnName + " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.");
                                    }
                                }
                                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_TOO_MANY)
                                {
                                    MessageBoxShow("Error: " + rightClickedOnName + " file has too many TBLU hash depends.");
                                }
                                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_HEADER_NOT_FOUND)
                                {
                                    MessageBoxShow("Error: " + rightClickedOnName + " file is an empty TEMP file, missing it's resource type header/footer.");
                                }
                                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_ENTRY_COUNT_MISMATCH)
                                {
                                    MessageBoxShow("Error: " + rightClickedOnName + " file and TBLU file have mismatched entry/entity counts.");
                                }
                                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_VERSION_UNKNOWN)
                                {
                                    MessageBoxShow("Error: " + rightClickedOnName + " file's version is unknown.");
                                }
                                else if (temp_progress.task_status == (int)Progress.RPKGStatus.TBLU_VERSION_UNKNOWN)
                                {
                                    MessageBoxShow("Error: " + rightClickedOnName + " file's TBLU file's version is unknown.");
                                }

                                temp_return_value = clear_temp_tblu_data();
                            }
                            else
                            {
                                if (entityBrickEditor == null)
                                {
                                    entityBrickEditor = new EntityBrickEditor();
                                }

                                entityBrickEditor.inputFolder = userSettings.InputFolder;
                                entityBrickEditor.outputFolder = userSettings.OutputFolder;

                                entityBrickEditor.temps_index = (UInt32)temp_index;
                                entityBrickEditor.tempFileName = rightClickedOnName;
                                entityBrickEditor.rpkgFilePath = rpkgFilePath;
                                entityBrickEditor.tempFileNameFull = rightClickedOnFullName;

                                string[] theme = userSettings.ColorTheme.Split('/');

                                entityBrickEditor.currentThemeBrightness = theme[0];
                                string color = theme[1];

                                entityBrickEditor.ShowDialog();

                                GC.Collect();
                                GC.WaitForPendingFinalizers();
                                GC.Collect();
                            }

                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }
                        else if (hashType == "WWEM")
                        {
                            command = "-extract_wwem_to_ogg_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To OGG To IOI Path...";
                        }
                        else if (hashType == "WWES")
                        {
                            command = "-extract_wwes_to_ogg_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To OGG To IOI Path...";
                        }
                        else if (hashType == "WWEV")
                        {
                            command = "-extract_wwev_to_ogg_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To OGG To IOI Path...";
                        }
                        else if (hashType == "DLGE")
                        {
                            command = "-extract_dlge_to_json_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To JSON...";
                        }
                        else if (hashType == "LOCR")
                        {
                            command = "-extract_locr_to_json_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To JSON...";
                        }
                        else if (hashType == "RTLV")
                        {
                            command = "-extract_rtlv_to_json_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To JSON...";
                        }
                        else if (hashType == "GFXI")
                        {
                            command = "-extract_ores_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To Image To IOI Path...";
                        }
                        else if (hashType == "JSON")
                        {
                            command = "-extract_ores_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To JSON To IOI Path...";
                        }

                        string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                        if (outputFolder == "")
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        output_path = outputFolder;
                    }
                    else if (rightClickMenu.buttonPressed == "button2" && buttonCount >= 4)
                    {
                        if (hashType == "PRIM")
                        {
                            command = "-extract_prim_to_glb_from";

                            progress.operation = (int)Progress.Operation.GENERAL;

                            filter = hashValue;

                            progress.message.Content = "Extracting " + rightClickedOnName + " To GLB File...";
                        }
                        else if (hashType == "TEMP")
                        {
                            string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

                            if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                            {
                                MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform PRIM model extraction.");

                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            List<string> rpkgFiles = new List<string>();

                            foreach (var filePath in Directory.GetFiles(runtimeDirectory))
                            {
                                if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                                {
                                    rpkgFiles.Add(filePath);
                                }
                            }

                            rpkgFiles.Sort(new NaturalStringComparer());

                            foreach (string filePath in rpkgFiles)
                            {
                                ImportRPKGFile(filePath);
                            }

                            command = "-extract_all_prim_model_of_temp_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting PRIM Models linked to " + rightClickedOnName + " To GLB/TGA File(s)...";

                            string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                            if (temp_outputFolder == "")
                            {
                                rightClickedOn = 0;

                                rightClickedOnFirst = true;

                                return;
                            }

                            output_path = temp_outputFolder;

                            int temp_return_value = reset_task_status();

                            execute_task temp_rpkgExecute = RebuildMODEL;

                            IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                            progress.operation = (int)Progress.Operation.PRIM_MODEL_EXTRACT;

                            progress.ShowDialog();

                            if (progress.task_status != (int)Progress.RPKGStatus.PRIM_MODEL_EXTRACT_SUCCESSFUL)
                            {
                                MessageBoxShow(progress.task_status_string.Replace("_", "__"));
                            }
                            else
                            {
                                MessageBoxShow("PRIM model(s) extracted successfully in " + temp_outputFolder);
                            }

                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                        if (outputFolder == "")
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        output_path = outputFolder;
                    }
                    else if (rightClickMenu.buttonPressed == "button3" && buttonCount == 5)
                    {
                        if (hashType == "TEMP")
                        {
                            command = "-extract_all_prim_of_temp_from";

                            filter = hashValue;

                            progress.message.Content = "Extracting PRIMs linked to " + rightClickedOnName + " To GLB File(s)...";
                        }

                        string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                        if (outputFolder == "")
                        {
                            rightClickedOn = 0;

                            rightClickedOnFirst = true;

                            return;
                        }

                        output_path = outputFolder;
                    }
                    else
                    {
                        rightClickedOn = 0;

                        rightClickedOnFirst = true;

                        return;
                    }

                    rightClickedOn = 0;

                    rightClickedOnFirst = true;

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        rightClickedOn = 0;

                        rightClickedOnFirst = true;

                        return;
                    }

                    rightClickedOn = 0;

                    rightClickedOnFirst = true;
                }
            }
            else
            {
                string[] headerSplit = header.Split(' ');

                string name = headerSplit[0];

                //MessageBoxShow(name);

                //MessageBoxShow(rightClickedOnFirst.ToString());

                if (name.Length == 4)
                {
                    if (rightClickedOn != 2)
                    {
                        rightClickedOn = 1;

                        rightClickedOnName = name;

                        rightClickedOnFullName = header;
                    }
                }
                else if (name.Contains("."))
                {
                    if (rightClickedOnFirst)
                    {
                        rightClickedOn = 2;

                        rightClickedOnName = name;

                        rightClickedOnFullName = header;

                        rightClickedOnFirst = false;
                    }
                }
            }
        }

        private void Extract_Click(object sender, RoutedEventArgs e)
        {
            var fileDialog = new Ookii.Dialogs.Wpf.VistaOpenFileDialog();

            fileDialog.Title = "Select an RPKG file to extract from:";

            fileDialog.Filter = "RPKG file|*.rpkg|All files|*.*";

            fileDialog.InitialDirectory = userSettings.InputFolder;

            fileDialog.FileName = userSettings.InputFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                ImportRPKGFile(fileDialog.FileName);

                //LoadHashDependsMap();
            }
            else
            {
                return;
            }

            string outputFolder = SelectFolder("output", "Select Output Folder To Extract RPKG To:");

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

            int return_value = reset_task_status();

            execute_task rpkgExecute = task_execute;

            IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

            progress.ShowDialog();

            if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
            {
                //MessageBoxShow(progress.task_status_string);

                return;
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

            string inputFolder = SelectFolder("input", "Select Input Folder (Runtime folder or other folder with multiple RPKGs) To Extract " + massExtractName + " From:");

            if (inputFolder == "")
            {
                return;
            }

            List<string> rpkgFiles = new List<string>();

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
            }

            string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + massExtractName + " To:");

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

                int return_value = reset_task_status();

                execute_task rpkgExecute = MassExtractTEXT;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                progress.ShowDialog();

                if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                {
                    //MessageBoxShow(progress.task_status_string);

                    return;
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

                int return_value = reset_task_status();

                execute_task rpkgExecute = task_execute;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                progress.ShowDialog();

                if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                {
                    //MessageBoxShow(progress.task_status_string);

                    return;
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

            string inputFolder = SelectFolder("input", "Select Folder To Rebuild " + rebuildType + " From:");

            if (inputFolder != "")
            {
                string command = "";
                string input_path = inputFolder;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = userSettings.OutputFolder;

                Progress progress = new Progress();

                progress.operation = (int)Progress.Operation.GENERAL;

                progress.ProgressBar.IsIndeterminate = true;

                if (rebuildType == "PRIM/TEXT/TEXD")
                {
                    command = "-rebuild_prim_model_in";

                    progress.message.Content = "Rebuilding All PRIM Models in " + input_path + "...";

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = RebuildMODEL;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.operation = (int)Progress.Operation.PRIM_MODEL_REBUILD;

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.PRIM_MODEL_REBUILD_SUCCESSFUL)
                    {
                        MessageBoxShow(progress.task_status_string.Replace("_", "__"));

                        /*if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_GLB_MESH_NAME_MALFORMED)
                        {
                            MessageBoxShow("Error: GLB mesh name is malformed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED)
                        {
                            MessageBoxShow("Error: Only one mesh primitive per mesh is allowed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3)
                        {
                            MessageBoxShow("Error: GLB vertex float size is not a multiple of 3.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_POSITION_DATA)
                        {
                            MessageBoxShow("Error: GLB is missing POSITION data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISMATCHED_BONES)
                        {
                            MessageBoxShow("Error: GLB has mismatched bones compared to the original BORG file.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM)
                        {
                            MessageBoxShow("Error: GLB weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_MISSING)
                        {
                            MessageBoxShow("Error: GLB is weighted, but does not have all necessary weighted data: JOINTS_0, WEIGHTS_0, JOINTS_1, WEIGHTS_1.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES)
                        {
                            MessageBoxShow("Error: GLB vertex float size not equal to normal float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_NORMAL_DATA)
                        {
                            MessageBoxShow("Error: GLB is missing NORMAL data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES)
                        {
                            MessageBoxShow("Error: GLB vertex float size not equal to UV float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_UV_DATA)
                        {
                            MessageBoxShow("Error: GLB is missing TEXCOORD_0 data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES)
                        {
                            MessageBoxShow("Error: GLB color size not equal to vertex size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_WRONG_FORMAT)
                        {
                            MessageBoxShow("Error: GLB color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS)
                        {
                            MessageBoxShow("Error: PRIM has too many primary object headers.");
                        }*/

                        return_value = clear_temp_tblu_data();

                        return;
                    }
                    else
                    {
                        MessageBoxShow("PRIM model(s) rebuilt successfully in " + inputFolder);
                    }
                }
                else if (rebuildType == "TEXT")
                {
                    command = "-rebuild_text_in";

                    progress.message.Content = "Rebuilding All TEXT/TEXD in " + input_path + "...";

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = RebuildTEXT;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        return;
                    }
                }
                else if (rebuildType == "PRIM")
                {
                    command = "-rebuild_prim_in";

                    progress.message.Content = "Rebuilding All PRIM in " + input_path + "...";

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = RebuildMODEL;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.operation = (int)Progress.Operation.PRIM_REBUILD;

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.PRIM_REBUILD_SUCCESSFUL)
                    {
                        MessageBoxShow(progress.task_status_string.Replace("_", "__"));

                        /*if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_GLB_MESH_NAME_MALFORMED)
                        {
                            MessageBoxShow("Error: GLB mesh name is malformed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED)
                        {
                            MessageBoxShow("Error: Only one mesh primitive per mesh is allowed.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3)
                        {
                            MessageBoxShow("Error: GLB vertex float size is not a multiple of 3.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_POSITION_DATA)
                        {
                            MessageBoxShow("Error: GLB is missing POSITION data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISMATCHED_BONES)
                        {
                            MessageBoxShow("Error: GLB has mismatched bones compared to the original BORG file.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM)
                        {
                            MessageBoxShow("Error: GLB weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_MISSING)
                        {
                            MessageBoxShow("Error: GLB is weighted, but does not have all necessary weighted data: JOINTS_0, WEIGHTS_0, JOINTS_1, WEIGHTS_1.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES)
                        {
                            MessageBoxShow("Error: GLB vertex float size not equal to normal float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_NORMAL_DATA)
                        {
                            MessageBoxShow("Error: GLB is missing NORMAL data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES)
                        {
                            MessageBoxShow("Error: GLB vertex float size not equal to UV float size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_MISSING_UV_DATA)
                        {
                            MessageBoxShow("Error: GLB is missing TEXCOORD_0 data.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES)
                        {
                            MessageBoxShow("Error: GLB color size not equal to vertex size.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_COLORS_WRONG_FORMAT)
                        {
                            MessageBoxShow("Error: GLB color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.");
                        }
                        else if (progress.task_status == (int)Progress.RPKGStatus.PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS)
                        {
                            MessageBoxShow("Error: PRIM has too many primary object headers.");
                        }*/

                        return_value = clear_temp_tblu_data();

                        return;
                    }
                    else
                    {
                        MessageBoxShow("PRIM(s) rebuilt successfully in " + inputFolder);
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

                    int return_value = reset_task_status();

                    execute_task rpkgExecute = task_execute;

                    IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        MessageBoxShow(progress.task_status_string);

                        return;
                    }
                }
            }
        }

        private void OpenRPKGFolder_Click(object sender, RoutedEventArgs e)
        {
            string inputFolder = SelectFolder("input", "Select Folder Containing RPKG Files To Import From:");

            if (inputFolder != "")
            {
                List<string> rpkgFiles = new List<string>();

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
                }
            }
        }

        private void GenerateRPKGFromFolder_Click(object sender, RoutedEventArgs e)
        {
            string inputFolder = SelectFolder("input", "Select Folder To Generate RPKG From:");

            if (inputFolder != "")
            {
                string outputFolder = SelectFolder("output", "Select Output Folder To Save Generated RPKG To:");

                string command = "-generate_rpkg_from";
                string input_path = inputFolder;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = outputFolder;

                string rpkgFile = inputFolder.Substring(inputFolder.LastIndexOf("\\") + 1);

                execute_task rpkgExecute = task_execute;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                Progress progress = new Progress();

                progress.message.Content = "Generating RPKG file " + rpkgFile + "...";

                progress.ShowDialog();
            }
        }

        private string SelectFolder(string type, string description)
        {
            var folderDialog = new Ookii.Dialogs.Wpf.VistaFolderBrowserDialog();

            folderDialog.Description = description;

            folderDialog.UseDescriptionForTitle = true;

            if (type == "input")
            {
                folderDialog.SelectedPath = userSettings.InputFolder;
            }
            else if (type == "output")
            {
                folderDialog.SelectedPath = userSettings.OutputFolder;
            }

            var folderDialogResult = folderDialog.ShowDialog();

            if (folderDialogResult == true)
            {
                if (type == "input")
                {
                    userSettings.InputFolder = folderDialog.SelectedPath;
                }
                else if (type == "output")
                {
                    userSettings.OutputFolder = folderDialog.SelectedPath;
                }

                var options = new JsonSerializerOptions { WriteIndented = true };

                string jsonString = JsonSerializer.Serialize(userSettings, options);

                File.WriteAllText("rpkg.json", jsonString);

                return folderDialog.SelectedPath;
            }

            return "";
        }

        private void ChangeColorTheme_Click(object sender, RoutedEventArgs e)
        {
            var button = (sender as MenuItem);

            string[] header = (button.Header as string).Split('/');

            string brightness = header[0];
            string color = header[1];

            SetColorTheme(brightness, color);

            userSettings.ColorTheme = (button.Header as string);

            var options = new JsonSerializerOptions { WriteIndented = true };

            string jsonString = JsonSerializer.Serialize(userSettings, options);

            File.WriteAllText("rpkg.json", jsonString);
        }

        private void LoadHashDependsMap()
        {
            foreach (TreeViewItem rpkgItem in MainTreeView.Items)
            {
                string rpkgPath = rpkgItem.Header.ToString();

                bool alreadyLoaded = false;

                foreach (TreeViewItem treeviewItem in HashMapTreeView.Items)
                {
                    if (rpkgPath == (treeviewItem.Header as string))
                    {
                        alreadyLoaded = true;
                    }
                }

                if (!alreadyLoaded)
                {
                    var item = new TreeViewItem();

                    item.Header = rpkgPath;

                    int return_value = reset_task_status();

                    execute_get_hashes_with_no_reverse_depends rpkg = get_hashes_with_no_reverse_depends;

                    IAsyncResult ar = rpkg.BeginInvoke(rpkgPath, null, null);

                    Progress progress = new Progress();

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    progress.ShowDialog();

                    string hashesWithNoReveserseDepends = Marshal.PtrToStringAnsi(get_hashes_with_no_reverse_depends_string());

                    string[] hashes = hashesWithNoReveserseDepends.TrimEnd(',').Split(',');

                    string rpkgFile = rpkgPath.Substring(rpkgPath.LastIndexOf("\\") + 1);

                    foreach (string hash in hashes)
                    {
                        var item2 = new TreeViewItem();

                        string[] temp_test = hash.Split('.');

                        string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

                        item2.Header = hash + " (" + rpkgFile + ") " + ioiString;

                        item2.Items.Add(null);

                        item2.Expanded += Item_Expanded;

                        item.Items.Add(item2);
                    }

                    HashMapTreeView.Items.Add(item);
                }
            }
        }

        private void Item_Expanded(object sender, RoutedEventArgs e)
        {
            var item = (sender as TreeViewItem);

            if (item.Items.Count == 1 && item.Items[0] == null)
            {
                item.Items.Clear();

                TreeViewItem parentPrevious = FindParentTreeViewItem(item);

                TreeViewItem parentCurrent = parentPrevious;

                while (parentCurrent != null)
                {
                    parentPrevious = parentCurrent;

                    parentCurrent = FindParentTreeViewItem(parentPrevious);
                }

                //System.Windows.MessageBox.Show(parentPrevious.Header.ToString());

                rpkgFilePath = parentPrevious.Header.ToString();

                string[] header = (item.Header as string).Replace("(", "").Replace(")", "").Split(' ');

                string hash = header[0];
                string rpkgFile = header[1];

                //System.Windows.MessageBox.Show(hash + "," + rpkgFilePath);

                int return_value = reset_task_status();

                execute_get_direct_hash_depends rpkgExecute = get_direct_hash_depends;

                IAsyncResult ar = rpkgExecute.BeginInvoke(rpkgFilePath, hash, null, null);

                Progress progress = new Progress();

                progress.operation = (int)Progress.Operation.GENERAL;

                progress.ShowDialog();

                string hashDependsString = Marshal.PtrToStringAnsi(get_direct_hash_depends_string());

                //System.Windows.MessageBox.Show(hashDependsString);

                string[] hashDepends = hashDependsString.TrimEnd(',').Split(',');

                if (hashDependsString != "")
                {
                    foreach (string hashDepend in hashDepends)
                    {
                        string[] hashData = hashDepend.Split('|');

                        string hash2 = hashData[0];
                        string rpkgFile2 = hashData[1];

                        var item2 = new TreeViewItem();

                        string[] temp_test = hash2.Split('.');

                        string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

                        item2.Header = hash2 + " (" + rpkgFile + ") " + ioiString;

                        item2.Items.Add(null);

                        item2.Expanded += Item_Expanded;

                        item.Items.Add(item2);
                    }
                }
            }

            GC.Collect();
        }

        private TreeViewItem FindParentTreeViewItem(object child)
        {
            try
            {
                var parent = VisualTreeHelper.GetParent(child as DependencyObject);
                while ((parent as TreeViewItem) == null)
                {
                    parent = VisualTreeHelper.GetParent(parent);
                }
                return parent as TreeViewItem;
            }
            catch (Exception e)
            {
                return null;
            }
        }

        private void MessageBoxShow(string messageBoxString)
        {
            Message messageBox = new Message();

            messageBox.message.Content = messageBoxString;

            messageBox.ShowDialog();
        }

        private void SetColorTheme(string brightness, string color)
        {
            if (brightness == "Light" && color == "Blue")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.DodgerBlue)));
            if (brightness == "Light" && color == "Brown")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.BurlyWood)));
            if (brightness == "Light" && color == "Green")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Green)));
            if (brightness == "Light" && color == "Orange")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Orange)));
            if (brightness == "Light" && color == "Purple")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Purple)));
            if (brightness == "Light" && color == "Red")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Red)));
            if (brightness == "Light" && color == "Yellow")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.Yellow)));
            if (brightness == "Light" && color == "White")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.White)));
            if (brightness == "Dark" && color == "Blue")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.DodgerBlue)));
            if (brightness == "Dark" && color == "Brown")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.BurlyWood)));
            if (brightness == "Dark" && color == "Green")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Green)));
            if (brightness == "Dark" && color == "Orange")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Orange)));
            if (brightness == "Dark" && color == "Purple")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Purple)));
            if (brightness == "Dark" && color == "Red")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Red)));
            if (brightness == "Dark" && color == "Yellow")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.Yellow)));
            if (brightness == "Dark" && color == "White")
                ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Dark", Colors.White)));
        }

        private void About_Click(object sender, RoutedEventArgs e)
        {
            Licenses licenses = new Licenses();
            licenses.ShowDialog();
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private UserSettings userSettings;
        public string rpkgFilePath = "";
        private bool rightClickedOnFirst = true;
        private int rightClickedOn = 0;
        private string rightClickedOnName = "";
        private string rightClickedOnFullName = "";
        private bool oneOrMoreRPKGsHaveBeenImported = false;
        private string currentRPKGFilePath = "";
        private string currentHash = "";
        private string currentHashFileName = "";
        public string hashDependsRPKGFilePath = "";
        private System.Windows.Threading.DispatcherTimer searchRPKGsInputTimer;
        private System.Windows.Threading.DispatcherTimer searchHashListInputTimer;
        private System.Windows.Threading.DispatcherTimer OGGPlayerTimer;
        private NAudio.Wave.WaveOut waveOut;
        private MemoryStream pcmMemoryStream;
        private int pcmSampleSize;
        private int pcmSampleRate;
        private int pcmChannels;
        EntityBrickEditor entityBrickEditor;
        List<string> patchHashOriginalList;
        List<string> patchHashList;
        List<string> patchHashNamesList;
        List<string> hashDependsOriginalList;
        List<string> hashDependsList;
        List<string> hashDependsNamesList;
        List<string> hashDependsFlagsList;
        int hashDependsPage;

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
        private bool oggPlayerPaused = false;
        private bool oggPlayerStopped = false;
        private bool oggPlayerStoppedNew = false;
        FileInfo file;
        FileStream stream;
        WaveFormat waveFormat;
        NAudio.Wave.RawSourceWaveStream pcmSource;

        public delegate int execute_load_recursive_temps(string temp_hash, string rpkg_file_path);
        public delegate int execute_get_hashes_with_no_reverse_depends(string rpkg_file);
        public delegate int execute_get_direct_hash_depends(string rpkg_file, string hash_string);
        public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "task_execute", CallingConvention = CallingConvention.Cdecl)]
        public static extern int task_execute(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "get_current_percent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_current_percent();

        [DllImport("rpkg.dll", EntryPoint = "reset_task_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_status();

        [DllImport("rpkg.dll", EntryPoint = "reset_task_single_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_single_status();

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_resource_types_count(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_resource_types_at(string rpkg_file, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_data_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 get_resource_types_data_size(string rpkg_file, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_based_on_resource_type_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_hash_based_on_resource_type_count(string rpkg_file, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_based_on_resource_type_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_based_on_resource_type_at(string rpkg_file, string resource_type, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_all_hashes_in_rpkg_count(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_hashes_in_rpkg_at(string rpkg_file, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_details", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_details(string rpkg_file, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_hash_in_rpkg_size(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_in_rpkg_data(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_data_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 get_all_hashes_in_rpkg_data_size(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "clear_hash_data_vector", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_hash_data_vector();

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_data_in_hex_view", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_in_rpkg_data_in_hex_view(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "generate_localization_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 generate_localization_string(string rpkg_file_name, string hash_string, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_localization_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_localization_string();

        [DllImport("rpkg.dll", EntryPoint = "generate_json_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 generate_json_string(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_json_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_json_string();

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hashes_with_no_reverse_depends();

        [DllImport("rpkg.dll", EntryPoint = "load_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int load_hash_list();

        [DllImport("rpkg.dll", EntryPoint = "get_hash_list_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_list_string(string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_hashes_with_no_reverse_depends(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hashes_with_no_reverse_depends_string();

        [DllImport("rpkg.dll", EntryPoint = "get_direct_hash_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_direct_hash_depends(string rpkg_file, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_direct_hash_depends_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_direct_hash_depends_string();

        [DllImport("rpkg.dll", EntryPoint = "get_patch_deletion_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_patch_deletion_list(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "search_imported_hashes", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_imported_hashes(string search_str, string rpkg_file, string resource_type, int max_results);

        [DllImport("rpkg.dll", EntryPoint = "get_search_imported_hashes", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_search_imported_hashes();

        [DllImport("rpkg.dll", EntryPoint = "search_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_hash_list(string search_str, int max_results);

        [DllImport("rpkg.dll", EntryPoint = "get_search_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_search_hash_list();

        [DllImport("rpkg.dll", EntryPoint = "get_rpkg_file_paths_hash_is_in", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_rpkg_file_paths_hash_is_in(string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "create_ogg_file_from_hash_in_rpkg", CallingConvention = CallingConvention.Cdecl)]
        public static extern int create_ogg_file_from_hash_in_rpkg(string rpkg_file, string hash_string, int command, int wwev_number);

        [DllImport("rpkg.dll", EntryPoint = "convert_ogg_to_pcm", CallingConvention = CallingConvention.Cdecl)]
        public static extern int convert_ogg_to_pcm(string input_path, string output_path);

        [DllImport("rpkg.dll", EntryPoint = "get_pcm_sample_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_pcm_sample_size();

        [DllImport("rpkg.dll", EntryPoint = "get_pcm_sample_rate", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_pcm_sample_rate();

        [DllImport("rpkg.dll", EntryPoint = "get_pcm_channels", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_pcm_channels();

        [DllImport("rpkg.dll", EntryPoint = "clear_temp_tblu_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_temp_tblu_data();

        [DllImport("resourcetool.dll", EntryPoint = "convert_temp_to_json", CallingConvention = CallingConvention.Cdecl)]
        public static extern int convert_temp_to_json(string input_path, string output_path, string operating_mode);

        [DllImport("rpkg.dll", EntryPoint = "get_matrix_data_from_godot_scene", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_matrix_data_from_godot_scene(string input_path);

        [DllImport("rpkg.dll", EntryPoint = "load_recursive_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int load_recursive_temps(string temp_hash, string rpkg_file_path);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_index(string temp_hash_string);

        [DllImport("rpkg.dll", EntryPoint = "generate_png_from_text", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_png_from_text(string rpkg_file, string hash_string, string png_path);

        [DllImport("rpkg.dll", EntryPoint = "unload_rpkg", CallingConvention = CallingConvention.Cdecl)]
        public static extern int unload_rpkg(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "modify_patch_deletion_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int modify_patch_deletion_list(string rpkg_file, string patch_list, UInt32 patch_count, UInt32 backup_rpkg);

        [DllImport("rpkg.dll", EntryPoint = "modify_hash_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern int modify_hash_depends(string rpkg_file, string hash_string, string hash_list, string hash_flag_list, UInt32 hash_count, UInt32 backup_rpkg);


        [SuppressUnmanagedCodeSecurity]
        internal static class SafeNativeMethods
        {
            [DllImport("shlwapi.dll", CharSet = CharSet.Unicode)]
            public static extern int StrCmpLogicalW(string psz1, string psz2);
        }

        public sealed class NaturalStringComparer : IComparer<string>
        {
            public int Compare(string a, string b)
            {
                return SafeNativeMethods.StrCmpLogicalW(a, b);
            }
        }

        public sealed class NaturalFileInfoNameComparer : IComparer<FileInfo>
        {
            public int Compare(FileInfo a, FileInfo b)
            {
                return SafeNativeMethods.StrCmpLogicalW(a.Name, b.Name);
            }
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            Process.GetCurrentProcess().Kill();
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

            SearchRPKGsTreeView.Items.Clear();

            if (SearchRPKGsTextBox.Text.Length > 0)
            {
                int maxSearchResults = 0;

                int.TryParse(SearchRPKGsComboBox.Text, out maxSearchResults);

                if (maxSearchResults == 0)
                {
                    maxSearchResults = 100;
                }

                int rpkgItemIndex = 0;

                foreach (TreeViewItem mainTreeViewItem in MainTreeView.Items)
                {
                    string rpkgPath = mainTreeViewItem.Header.ToString();

                    bool rpkgItemAdded = false;

                    //MessageBoxShow(rpkgPath);

                    int rpkgResultsCount = 0;

                    int resourceTypeItemIndex = 0;

                    foreach (TreeViewItem resourceItems in mainTreeViewItem.Items)
                    {
                        if (rpkgResultsCount <= maxSearchResults)
                        {
                            string resourceType = resourceItems.Header.ToString();

                            bool resourceTypeItemAdded = false;

                            //MessageBoxShow(resourceType);

                            int return_value = search_imported_hashes(SearchRPKGsTextBox.Text, rpkgPath, resourceType, maxSearchResults);

                            //MessageBoxShow(Marshal.PtrToStringAnsi(get_search_imported_hashes()));

                            string searchResultsString = Marshal.PtrToStringAnsi(get_search_imported_hashes());

                            bool found = false;

                            TreeViewItem item;

                            if (!rpkgItemAdded)
                            {
                                item = new TreeViewItem();
                            }
                            else
                            {
                                item = (SearchRPKGsTreeView.Items.GetItemAt(rpkgItemIndex) as TreeViewItem);
                            }

                            item.Header = rpkgPath;

                            //item.IsExpanded = true;

                            string rpkgFile = rpkgPath.Substring(rpkgPath.LastIndexOf("\\") + 1);

                            string[] searchResults = searchResultsString.TrimEnd(',').Split(',');

                            if (searchResults.Length > 0)
                            {
                                foreach (string searchResult in searchResults)
                                {
                                    if (searchResult != "")
                                    {
                                        found = true;

                                        rpkgResultsCount++;

                                        //MessageBoxShow(searchResult);

                                        TreeViewItem item2;

                                        if (!resourceTypeItemAdded)
                                        {
                                            item2 = new TreeViewItem();
                                        }
                                        else
                                        {
                                            item2 = (item.Items.GetItemAt(resourceTypeItemIndex) as TreeViewItem);
                                        }

                                        item2.Header = resourceType;

                                        item2.IsExpanded = true;

                                        //var item3 = new TreeViewItem();

                                        string[] temp_test = searchResult.Split('.');

                                        string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

                                        //item3.Header = searchResult + " (" + rpkgFile + ") " + ioiString;

                                        item2.Items.Add(searchResult + " (" + rpkgFile + ") " + ioiString);

                                        //item3.Expanded += Item_Expanded;

                                        //item2.Items.Add(item3);

                                        if (!resourceTypeItemAdded)
                                        {
                                            item.Items.Add(item2);

                                            resourceTypeItemAdded = true;
                                        }
                                    }
                                }

                                if (!rpkgItemAdded && found)
                                {
                                    SearchRPKGsTreeView.Items.Add(item);

                                    rpkgItemAdded = true;
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

            SearchHashListTreeView.Items.Clear();

            if (SearchHashListTextBox.Text.Length > 0)
            {
                int maxSearchResults = 0;

                int.TryParse(SearchHashListComboBox.Text, out maxSearchResults);

                if (maxSearchResults == 0)
                {
                    maxSearchResults = 100;
                }

                int return_value = search_hash_list(SearchHashListTextBox.Text, maxSearchResults);

                string searchResultsString = Marshal.PtrToStringAnsi(get_search_hash_list());

                string[] searchResults = searchResultsString.TrimEnd(',').Split(',');

                if (searchResults.Length > 0)
                {
                    foreach (string searchResult in searchResults)
                    {
                        if (searchResult != "")
                        {
                            string[] temp_test = searchResult.Split('.');

                            string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

                            SearchHashListTreeView.Items.Add(searchResult + " (hashlist) " + ioiString);
                        }
                    }
                }
            }
        }

        private void OGGPlayerTimer_Tick(object sender, EventArgs e)
        {
            //MessageBoxShow((((double)waveStream.Position / (double)waveStream.Length) * 100.0).ToString());
            OGGPlayer.Value = ((double)pcmSource.Position / (double)pcmSource.Length) * 100.0;
            OGGPlayerLabel.Content = pcmSource.CurrentTime.ToString() + " / " + pcmSource.TotalTime.ToString();

            if (pcmSource.Position == pcmSource.Length)
            {
                if (OGGPlayerTimer == null)
                {
                    return;
                }

                OGGPlayerTimer.Stop();

                waveOut.Stop();

                //MessageBoxShow(waveStream.CurrentTime.ToString());

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

                    int return_value = create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hashData[0], 1, newIndex);

                    string currentDirectory = System.IO.Directory.GetCurrentDirectory();

                    string inputOGGFile = currentDirectory + "\\" + hashData[0] + ".ogg";

                    string outputPCMFile = currentDirectory + "\\" + hashData[0] + ".pcm";

                    return_value = convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

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

                            int pcmSampleSize = get_pcm_sample_size();
                            int pcmSampleRate = get_pcm_sample_rate();
                            int pcmChannels = get_pcm_channels();

                            File.Delete(hashData[0] + ".ogg");
                            File.Delete(hashData[0] + ".wem");
                            File.Delete(hashData[0] + ".pcm");

                            //file = new FileInfo("output.pcm");
                            //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                            waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                            pcmSource = new NAudio.Wave.RawSourceWaveStream(pcmMemoryStream, waveFormat);

                            oggPlayerState = (int)OggPlayerState.READY;
                        }
                    }
                    else
                    {
                        if (FifthTabRight.Visibility == Visibility.Visible)
                        {
                            FifthTabRight.Visibility = Visibility.Collapsed;
                        }

                        FirstTabRight.IsSelected = true;
                    }
                }
                else
                {
                    oggPlayerState = (int)OggPlayerState.RESET;
                }
            }

            //MessageBoxShow(pcmSource.Position.ToString() + "," + pcmSource.Length.ToString());
            //MessageBoxShow(pcmSource.CurrentTime.ToString() + " / " + pcmSource.TotalTime.ToString());
        }

        private void OGGPlayer_DragCompleted(object sender, System.Windows.Controls.Primitives.DragCompletedEventArgs e)
        {
            //MessageBoxShow(waveStream.Length.ToString() + "," + ((double)waveStream.Length * (OGGPlayer.Value / 100.0)).ToString() + "," + OGGPlayer.Value.ToString());

            //MessageBoxShow(waveStream.CurrentTime.ToString());

            pcmSource.Position = (long)((double)pcmSource.Length * (OGGPlayer.Value / 100.0));

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

            //MessageBoxShow(waveStream.CurrentTime.ToString());

            pcmSource.Position = (long)((double)pcmSource.Length * (OGGPlayer.Value / 100.0));

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
                    NAudio.Wave.MultiplexingWaveProvider multiplexingWaveProvider;
                    multiplexingWaveProvider = new MultiplexingWaveProvider(new IWaveProvider[] { pcmSource }, 2);
                    multiplexingWaveProvider.ConnectInputToOutput(0, 0);
                    multiplexingWaveProvider.ConnectInputToOutput(0, 1);
                    var wait = new System.Threading.ManualResetEventSlim(false);
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
                        MessageBoxShow("OGG File can't be played due to WaveBadFormat error.");
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

                //MessageBoxShow(waveStream.CurrentTime.ToString());

                OGGPlayer.Value = 0;

                pcmSource.Position = 0;

                oggPlayerPaused = false;

                oggPlayerStopped = true;

                OGGPlayerPlay.Kind = MahApps.Metro.IconPacks.PackIconMaterialDesignKind.PlayCircleOutline;

                string[] hashData = OGGPlayerLabelHashFileName.Content.ToString().Split('.');

                int return_value = create_ogg_file_from_hash_in_rpkg(rpkgFilePath, hashData[0], 1, OGGPlayerComboBox.SelectedIndex);

                string currentDirectory = System.IO.Directory.GetCurrentDirectory();

                string inputOGGFile = currentDirectory + "\\" + hashData[0] + ".ogg";

                string outputPCMFile = currentDirectory + "\\" + hashData[0] + ".pcm";

                return_value = convert_ogg_to_pcm(inputOGGFile, outputPCMFile);

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

                        int pcmSampleSize = get_pcm_sample_size();
                        int pcmSampleRate = get_pcm_sample_rate();
                        int pcmChannels = get_pcm_channels();

                        File.Delete(hashData[0] + ".ogg");
                        File.Delete(hashData[0] + ".wem");
                        File.Delete(hashData[0] + ".pcm");

                        //file = new FileInfo("output.pcm");
                        //stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.Read);

                        waveFormat = new WaveFormat(pcmSampleRate, 16, pcmChannels);

                        pcmSource = new NAudio.Wave.RawSourceWaveStream(pcmMemoryStream, waveFormat);

                        oggPlayerState = (int)OggPlayerState.RESET;
                    }
                }
                else
                {
                    if (FifthTabRight.Visibility == Visibility.Visible)
                    {
                        FifthTabRight.Visibility = Visibility.Collapsed;
                    }

                    FirstTabRight.IsSelected = true;
                }
            }
        }

        private void EntityBrickEditor_Click(object sender, RoutedEventArgs e)
        {
            if (entityBrickEditor == null)
            {
                entityBrickEditor = new EntityBrickEditor();
            }

            entityBrickEditor.inputFolder = userSettings.InputFolder;
            entityBrickEditor.outputFolder = userSettings.OutputFolder;

            entityBrickEditor.tempFileName = "00E9F09C3B030590.TEMP";
            entityBrickEditor.rpkgFilePath = "C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk26patch1.rpkg";

            entityBrickEditor.ShowDialog();

            GC.Collect();
            GC.WaitForPendingFinalizers();
            GC.Collect();
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            string input_path = "R:\\testing\\00E9F09C3B030590.TEMP";
            string output_path = "R:\\testing\\00E9F09C3B030590.OUTPUT.TEMP";

            //int return_value = convert_temp_to_json(input_path, output_path, "convert");
        }

        private void DetailsTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void EditPatchDeletionListButton_Click(object sender, RoutedEventArgs e)
        {
            string[] lines = DetailsTextBox.Text.Split('\n');

            bool patchListFound = false;

            patchHashOriginalList = new List<string>();
            patchHashList = new List<string>();
            patchHashNamesList = new List<string>();

            int patchNumber = 0;

            foreach (string line in lines)
            {
                if (patchListFound)
                {
                    string[] patchData = line.Split(' ');

                    if (patchData.Length > 3)
                    {
                        string[] patchHashData = patchData[3].Split('.');

                        patchHashOriginalList.Add(patchHashData[0]);

                        patchHashList.Add(patchHashData[0]);

                        patchHashNamesList.Add(line.Replace("  - ", ""));
                    }

                    //MessageBoxShow(patchData[3]);
                }

                if (line.Contains("Patch file has"))
                {
                    string[] patchData = line.Split(' ');

                    int.TryParse(patchData[3], out patchNumber);

                    if (patchNumber > 0)
                    {
                        patchListFound = true;

                        //MessageBoxShow(patchNumber.ToString());
                    }
                }
            }

            if (patchNumber != patchHashList.Count)
            {
                MessageBoxShow("Error: Parsing the Details textbox for the patch hash values.");

                return;
            }

            DetailsTextBox.Visibility = Visibility.Collapsed;
            MainScrollViewer.Visibility = Visibility.Visible;
            MainStackPanel.Visibility = Visibility.Visible;
            EditPatchDeletionListButton.Visibility = Visibility.Collapsed;
            SaveChangesToPatchDeletionListButton.Visibility = Visibility.Visible;
            DiscardChangesToPatchDeletionListButton.Visibility = Visibility.Visible;

            LoadPatchDeletionEditor();
        }

        private void LoadPatchDeletionEditor()
        {
            while (MainStackPanel.Children.Count > 0)
            {
                MainStackPanel.Children.RemoveAt(MainStackPanel.Children.Count - 1);
            }

            Label label1 = new Label();
            label1 = new Label();
            label1.Content = "Patch Hash Deletion List:";
            label1.FontSize = 16;
            label1.FontWeight = FontWeights.Bold;
            MainStackPanel.Children.Add(label1);

            ToggleSwitch toggleSwitch = new ToggleSwitch();
            toggleSwitch.Name = "backup_rpkg";
            toggleSwitch.Content = "Backup RPKG File When Saving Changes";
            toggleSwitch.IsOn = false;
            toggleSwitch.HorizontalAlignment = HorizontalAlignment.Center;
            MainStackPanel.Children.Add(toggleSwitch);

            Button button1 = new Button();
            button1.Name = "patch_add";
            button1.Content = "Add New Patch Deletion List Entry";
            button1.Width = 240.0;
            button1.Margin = new Thickness(0, 8, 0, 4);
            button1.Click += PatchAdd_Click;
            MainStackPanel.Children.Add(button1);

            button1 = new Button();
            button1.Name = "patch_delete_all";
            button1.Content = "Remove All";
            button1.Width = 100.0;
            button1.Margin = new Thickness(0, 4, 0, 8);
            button1.Click += PatchDelete_Click;
            MainStackPanel.Children.Add(button1);

            int count = 0;

            foreach (string patchHash in patchHashList)
            {
                Grid grid = new Grid();

                RowDefinition rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(8);
                grid.RowDefinitions.Add(rowDefinition);
                rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                grid.RowDefinitions.Add(rowDefinition);
                rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(8);
                grid.RowDefinitions.Add(rowDefinition);
                ColumnDefinition columnDefinition = new ColumnDefinition();
                columnDefinition.Width = GridLength.Auto;
                grid.ColumnDefinitions.Add(columnDefinition);
                columnDefinition = new ColumnDefinition();
                columnDefinition.Width = new GridLength(1, GridUnitType.Star);
                grid.ColumnDefinitions.Add(columnDefinition);
                columnDefinition = new ColumnDefinition();
                columnDefinition.Width = GridLength.Auto;
                grid.ColumnDefinitions.Add(columnDefinition);

                Label label = new Label();
                label.Content = "Entry " + count.ToString() + ": ";
                grid.Children.Add(label);
                Grid.SetRow(label, 1);
                Grid.SetColumn(label, 0);

                label = new Label();
                label.Content = patchHashNamesList[count];
                label.FontSize = 12;
                MainStackPanel.Children.Add(label);

                TextBox textBox = new TextBox();
                textBox.Name = "patch_" + patchHash + "_" + count.ToString();
                textBox.Text = patchHash;
                textBox.Margin = new Thickness(4, 0, 8, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);

                Button button = new Button();
                button.Name = "patch_delete_" + patchHash + "_" + count.ToString();
                button.Content = "Remove";
                button.Click += PatchDelete_Click;
                button.Margin = new Thickness(4, 0, 8, 0);
                grid.Children.Add(button);
                Grid.SetRow(button, 1);
                Grid.SetColumn(button, 2);

                MainStackPanel.Children.Add(grid);

                count++;
            }
        }

        private void PatchAdd_Click(object sender, RoutedEventArgs e)
        {
            int count = patchHashList.Count;

            patchHashList.Add("00123456789ABDEF");

            patchHashNamesList.Add("New");

            Grid grid = new Grid();

            RowDefinition rowDefinition = new RowDefinition();
            rowDefinition.Height = new GridLength(8);
            grid.RowDefinitions.Add(rowDefinition);
            rowDefinition = new RowDefinition();
            rowDefinition.Height = new GridLength(1, GridUnitType.Star);
            grid.RowDefinitions.Add(rowDefinition);
            rowDefinition = new RowDefinition();
            rowDefinition.Height = new GridLength(8);
            grid.RowDefinitions.Add(rowDefinition);
            ColumnDefinition columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);

            Label label = new Label();
            label.Content = "Entry " + count.ToString() + ": ";
            grid.Children.Add(label);
            Grid.SetRow(label, 1);
            Grid.SetColumn(label, 0);

            label = new Label();
            label.Content = patchHashNamesList[count];
            label.FontSize = 12;
            MainStackPanel.Children.Add(label);

            TextBox textBox = new TextBox();
            textBox.Name = "patch_" + patchHashList[count] + "_" + count.ToString();
            textBox.Text = patchHashList[count];
            textBox.Margin = new Thickness(4, 0, 8, 0);
            grid.Children.Add(textBox);
            Grid.SetRow(textBox, 1);
            Grid.SetColumn(textBox, 1);

            Button button = new Button();
            button.Name = "patch_delete_" + patchHashList[count] + "_" + count.ToString();
            button.Content = "Remove";
            button.Click += PatchDelete_Click;
            button.Margin = new Thickness(4, 0, 8, 0);
            grid.Children.Add(button);
            Grid.SetRow(button, 1);
            Grid.SetColumn(button, 2);

            MainStackPanel.Children.Add(grid);
        }

        private void PatchDelete_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as Button).Name == "patch_delete_all")
            {
                patchHashList.Clear();

                patchHashNamesList.Clear();

                LoadPatchDeletionEditor();
            }
            else
            {
                //MessageBoxShow((sender as Button).Name);

                string[] patchToDeleteData = (sender as Button).Name.Split('_');

                int itemToDelete = 0;

                int.TryParse(patchToDeleteData[3], out itemToDelete);

                patchHashList.RemoveAt(itemToDelete);

                patchHashNamesList.RemoveAt(itemToDelete);

                LoadPatchDeletionEditor();
            }
        }

        bool IsLegalHashName(string hashName)
        {
            hashName.ToUpper();

            if (hashName.Length != 16)
            {
                return false;
            }

            string legalCharacters = "0123456789ABCDEF";

            foreach (char a in hashName)
            {
                bool charIsLegal = false;

                foreach (char b in legalCharacters)
                {
                    if (a == b)
                    {
                        charIsLegal = true;
                    }
                }

                if (!charIsLegal)
                {
                    return false;
                }
            }

            return true;
        }

        bool IsLegalHashFlag(string hashFlag)
        {
            hashFlag.ToUpper();

            if (hashFlag.Length != 2)
            {
                return false;
            }

            string legalCharacters = "0123456789ABCDEF";

            foreach (char a in hashFlag)
            {
                bool charIsLegal = false;

                foreach (char b in legalCharacters)
                {
                    if (a == b)
                    {
                        charIsLegal = true;
                    }
                }

                if (!charIsLegal)
                {
                    return false;
                }
            }

            return true;
        }

        private void SaveChangesToPatchDeletionListButton_Click(object sender, RoutedEventArgs e)
        {
            UInt32 patchNumber = 0;

            string patchList = "";

            bool backup_rpkg_file = false;

            foreach (var child in MainStackPanel.Children)
            {
                if ((child as Label) != null)
                {
                    //MessageBoxShow("Label: " + (child as Label).Content);
                }
                else if ((child as Grid) != null)
                {
                    foreach (var gridChild in (child as Grid).Children)
                    {
                        if ((gridChild as Label) != null)
                        {
                            //MessageBoxShow("Grid Label: " + (gridChild as Label).Content);
                        }
                        else if ((gridChild as TextBox) != null)
                        {
                            //MessageBoxShow("Grid TextBox Name: " + (gridChild as TextBox).Name + ", " + "Grid TextBox Name: " + (gridChild as TextBox).Text);

                            patchNumber++;

                            patchList += (gridChild as TextBox).Text;

                            if (!IsLegalHashName((gridChild as TextBox).Text))
                            {
                                MessageBoxShow("Error: " + (gridChild as TextBox).Text + " is not a valid hash value. Needs to be 16 characters long and consist of only 0-9 and A-F.");

                                return;
                            }
                        }
                    }
                }
                else if ((child as ToggleSwitch) != null)
                {
                    //MessageBoxShow("ToggleSwitch: " + (child as ToggleSwitch).IsOn.ToString());

                    if ((child as ToggleSwitch).IsOn)
                    {
                        backup_rpkg_file = true;
                    }
                }
            }

            if (patchNumber != patchHashList.Count)
            {
                MessageBoxShow("Error: Parsing the patch deletion textboxes for the patch hash values.");

                return;
            }

            MessageQuestion messageBox = new MessageQuestion();

            if (backup_rpkg_file)
            {
                messageBox.message.Content = "Are you sure you want to modify the patch deletion list and regenerate the RPKG file?\n\nRPKG file: " + rpkgFilePath + "\n\nThe original patch deletion list size: " + patchHashOriginalList.Count.ToString() + "\n\nThe new patch deletion list size: " + patchNumber + "\n\nThe backup RPKG toggle option was enabled, so the existing RPKG file will be backed up.";
            }
            else
            {
                messageBox.message.Content = "Are you sure you want to modify the patch deletion list and regenerate the RPKG file?\n\nRPKG file: " + rpkgFilePath + "\n\nThe original patch deletion list size: " + patchHashOriginalList.Count.ToString() + "\n\nThe new patch deletion list size: " + patchNumber + "\n\nThe backup RPKG toggle option was not enabled, so the existing RPKG file will not be backed up.";
            }

            messageBox.ShowDialog();

            if (messageBox.buttonPressed == "OKButton")
            {
                int return_value = 0;

                if (backup_rpkg_file)
                {
                    return_value = modify_patch_deletion_list(rpkgFilePath, patchList, patchNumber, 1);
                }
                else
                {
                    return_value = modify_patch_deletion_list(rpkgFilePath, patchList, patchNumber, 0);
                }

                if (return_value == 0)
                {
                    MessageBoxShow("Patch deletion list has been successfully updated for RPKG: " + rpkgFilePath);

                    return_value = unload_rpkg(rpkgFilePath);

                    SearchRPKGsTreeView.Items.Clear();

                    bool treeview_item_found = false;

                    int count = 0;

                    int treeview_item_index = 0;

                    foreach (TreeViewItem item in MainTreeView.Items)
                    {
                        if (item.Header.ToString() == rpkgFilePath)
                        {
                            treeview_item_index = count;

                            treeview_item_found = true;
                        }

                        count++;
                    }

                    if (treeview_item_found)
                    {
                        MainTreeView.Items.RemoveAt(treeview_item_index);

                        ImportRPKGFile(rpkgFilePath);

                        foreach (TreeViewItem item in MainTreeView.Items)
                        {
                            if (item.Header.ToString() == rpkgFilePath)
                            {
                                item.IsSelected = true;
                            }
                        }
                    }
                    else
                    {
                        MessageBoxShow("Error: Cound not find the treeview item for unloading for RPKG: " + rpkgFilePath);
                    }
                }
                else if (return_value == 1)
                {
                    MessageBoxShow("Error: Patch list from RPKG<->RPKG.DLL does not match for RPKG: " + rpkgFilePath);
                }
                else if (return_value == 2)
                {
                    MessageBoxShow("Error: RPKG is not a patch file: " + rpkgFilePath);
                }
                else if (return_value == 3)
                {
                    MessageBoxShow("Error: RPKG version error for RPKG: " + rpkgFilePath);
                }
                else if (return_value == 4)
                {
                    MessageBoxShow("Error: Can not open file RPKG: " + rpkgFilePath);
                }
                else if (return_value == 5)
                {
                    MessageBoxShow("Error: Can not create temporary file for RPKG: " + rpkgFilePath);
                }
                else if (return_value == 6)
                {
                    MessageBoxShow("Error: Could not delete the existing RPKG file: " + rpkgFilePath);
                }
            }
            else if (messageBox.buttonPressed == "CancelButton")
            {
                return;
            }
        }

        private void DiscardChangesToPatchDeletionListButton_Click(object sender, RoutedEventArgs e)
        {
            DetailsTextBox.Visibility = Visibility.Visible;
            MainScrollViewer.Visibility = Visibility.Collapsed;
            MainStackPanel.Visibility = Visibility.Collapsed;
            EditPatchDeletionListButton.Visibility = Visibility.Visible;
            SaveChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
            DiscardChangesToPatchDeletionListButton.Visibility = Visibility.Collapsed;
        }

        private void EditHashDependsButton_Click(object sender, RoutedEventArgs e)
        {
            string[] lines = DetailsTextBox.Text.Split('\n');

            bool hashDependsFound = false;

            hashDependsOriginalList = new List<string>();
            hashDependsList = new List<string>();
            hashDependsNamesList = new List<string>();
            hashDependsFlagsList = new List<string>();

            int hashDependsNumber = 0;

            foreach (string line in lines)
            {
                if (line.StartsWith("There are"))
                {
                    hashDependsFound = false;
                }

                if (hashDependsFound)
                {
                    string[] hashDependsData = line.Split(' ');

                    if (hashDependsData.Length > 3)
                    {
                        string[] hashDependsDataSplit = hashDependsData[3].Split('.');

                        hashDependsOriginalList.Add(hashDependsDataSplit[0]);

                        hashDependsList.Add(hashDependsDataSplit[0]);

                        hashDependsNamesList.Add(line.Replace("  - ", ""));

                        hashDependsFlagsList.Add(hashDependsData[4]);

                        //MessageBoxShow(hashDependsData[3]);
                    }
                }

                if (line.StartsWith("Depends on"))
                {
                    string[] hashDependsData = line.Split(' ');

                    int.TryParse(hashDependsData[2], out hashDependsNumber);

                    if (hashDependsNumber > 0)
                    {
                        hashDependsFound = true;

                        //MessageBoxShow(hashDependsNumber.ToString());
                    }
                }
            }

            if (hashDependsNumber != hashDependsList.Count)
            {
                MessageBoxShow("Error: Parsing the Details textbox for the hash depends values.");

                return;
            }

            DetailsTextBox.Visibility = Visibility.Collapsed;
            MainScrollViewer.Visibility = Visibility.Visible;
            MainStackPanel.Visibility = Visibility.Visible;
            EditHashDependsButton.Visibility = Visibility.Collapsed;
            SaveChangesToHashDependsButton.Visibility = Visibility.Visible;
            DiscardChangesToHashDependsButton.Visibility = Visibility.Visible;
            ExtractHashDependsButton.Visibility = Visibility.Collapsed;

            LoadHashDependsEditor(0);
        }

        private void LoadHashDependsEditor(int page)
        {
            hashDependsPage = page;

            int pageCountStart = page * 100;
            int pageCountEnd = pageCountStart + 100;

            //using (var suspend = Dispatcher.DisableProcessing())
            {
                while (MainStackPanel.Children.Count > 0)
                {
                    MainStackPanel.Children.RemoveAt(MainStackPanel.Children.Count - 1);
                }

                if (hashDependsList.Count == 0)
                {
                    Label label0 = new Label();
                    label0.Content = "No Hash Depends:";
                    label0.FontSize = 16;
                    label0.FontWeight = FontWeights.Bold;
                    label0.HorizontalAlignment = HorizontalAlignment.Center;
                    MainStackPanel.Children.Add(label0);

                    ToggleSwitch toggleSwitch0 = new ToggleSwitch();
                    toggleSwitch0.Name = "backup_rpkg";
                    toggleSwitch0.Content = "Backup RPKG File When Saving Changes";
                    toggleSwitch0.IsOn = false;
                    toggleSwitch0.HorizontalAlignment = HorizontalAlignment.Center;
                    MainStackPanel.Children.Add(toggleSwitch0);

                    Button button0 = new Button();
                    button0.Name = "hash_depends_add";
                    button0.Content = "Add New Hash Depends Entry";
                    button0.Width = 200.0;
                    button0.Margin = new Thickness(0, 8, 0, 4);
                    button0.Click += HashDependsAdd_Click;
                    MainStackPanel.Children.Add(button0);

                    return;
                }

                Label label1 = new Label();
                label1.Content = "Hash Depends:";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;
                label1.HorizontalAlignment = HorizontalAlignment.Center;
                MainStackPanel.Children.Add(label1);

                Button button1;

                if (page == 0)
                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(200.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    if (hashDependsList.Count > 100)
                    {
                        button1 = new Button();
                        button1.Name = "hash_depends_page_forward";
                        button1.Content = ">";
                        button1.FontWeight = FontWeights.Bold;
                        button1.Width = 100.0;
                        button1.Margin = new Thickness(0, 4, 0, 8);
                        button1.Click += HashDependsForward_Click;
                        grid1.Children.Add(button1);
                        Grid.SetRow(button1, 0);
                        Grid.SetColumn(button1, 2);
                    }

                    MainStackPanel.Children.Add(grid1);
                }
                else if (hashDependsList.Count >= pageCountStart && hashDependsList.Count <= pageCountEnd)
                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(200.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    button1 = new Button();
                    button1.Name = "hash_depends_page_back";
                    button1.Content = "<";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 100.0;
                    button1.Margin = new Thickness(0, 4, 0, 8);
                    button1.Click += HashDependsBack_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 0);

                    MainStackPanel.Children.Add(grid1);
                }
                else
                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(100.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    button1 = new Button();
                    button1.Name = "hash_depends_page_back";
                    button1.Content = "<";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 100.0;
                    button1.Margin = new Thickness(0, 4, 0, 8);
                    button1.Click += HashDependsBack_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 0);

                    button1 = new Button();
                    button1.Name = "hash_depends_page_forward";
                    button1.Content = ">";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 100.0;
                    button1.Margin = new Thickness(0, 4, 0, 8);
                    button1.Click += HashDependsForward_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 2);

                    MainStackPanel.Children.Add(grid1);
                }

                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(50.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(50.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    Label label = new Label();
                    label.Content = "Jump to hash depend number:";
                    grid1.Children.Add(label);
                    Grid.SetRow(label, 1);
                    Grid.SetColumn(label, 0);

                    TextBox textBox = new TextBox();
                    textBox.Name = "hash_depends_jump_to_1";
                    textBox.Text = "";
                    textBox.Margin = new Thickness(4, 0, 4, 0);
                    grid1.Children.Add(textBox);
                    Grid.SetRow(textBox, 1);
                    Grid.SetColumn(textBox, 1);

                    if (MainStackPanel.FindName("hash_depends_jump_to_1") == null)
                    {
                        RegisterName(textBox.Name, textBox);
                    }
                    else
                    {
                        UnregisterName(textBox.Name);

                        RegisterName(textBox.Name, textBox);
                    }

                    button1 = new Button();
                    button1.Name = "hash_depends_jump_1";
                    button1.Content = "Jump";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 60.0;
                    button1.Margin = new Thickness(4, 0, 4, 0);
                    button1.Click += HashDependsJump_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 2);

                    MainStackPanel.Children.Add(grid1);
                }
                                
                ToggleSwitch toggleSwitch = new ToggleSwitch();
                toggleSwitch.Name = "backup_rpkg";
                toggleSwitch.Content = "Backup RPKG File When Saving Changes";
                toggleSwitch.IsOn = false;
                toggleSwitch.HorizontalAlignment = HorizontalAlignment.Center;
                MainStackPanel.Children.Add(toggleSwitch);

                button1 = new Button();
                button1.Name = "hash_depends_add";
                button1.Content = "Add New Hash Depends Entry";
                button1.Width = 200.0;
                button1.Margin = new Thickness(0, 8, 0, 4);
                button1.Click += HashDependsAdd_Click;
                MainStackPanel.Children.Add(button1);

                button1 = new Button();
                button1.Name = "hash_depends_delete_all";
                button1.Content = "Remove All";
                button1.Width = 100.0;
                button1.Margin = new Thickness(0, 4, 0, 8);
                button1.Click += HashDependsDelete_Click;
                MainStackPanel.Children.Add(button1);

                int count = 0;
                int countActual = 0;

                //foreach (string hashDepend in hashDependsList)
                for (int h = 0; h < hashDependsList.Count; h++)
                {
                    if (count >= pageCountStart && count < pageCountEnd)
                    {
                        Grid grid = new Grid();

                        RowDefinition rowDefinition = new RowDefinition();
                        rowDefinition.Height = new GridLength(8);
                        grid.RowDefinitions.Add(rowDefinition);
                        rowDefinition = new RowDefinition();
                        rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                        grid.RowDefinitions.Add(rowDefinition);
                        rowDefinition = new RowDefinition();
                        rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                        grid.RowDefinitions.Add(rowDefinition);
                        rowDefinition = new RowDefinition();
                        rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                        grid.RowDefinitions.Add(rowDefinition);
                        ColumnDefinition columnDefinition = new ColumnDefinition();
                        columnDefinition.Width = GridLength.Auto;
                        grid.ColumnDefinitions.Add(columnDefinition);
                        columnDefinition = new ColumnDefinition();
                        columnDefinition.Width = new GridLength(1, GridUnitType.Star);
                        grid.ColumnDefinitions.Add(columnDefinition);
                        columnDefinition = new ColumnDefinition();
                        columnDefinition.Width = new GridLength(36.0);
                        grid.ColumnDefinitions.Add(columnDefinition);
                        columnDefinition = new ColumnDefinition();
                        columnDefinition.Width = GridLength.Auto;
                        grid.ColumnDefinitions.Add(columnDefinition);

                        Label label = new Label();
                        label.Content = "Hash";
                        grid.Children.Add(label);
                        Grid.SetRow(label, 1);
                        Grid.SetColumn(label, 1);

                        label = new Label();
                        label.Content = "Flag";
                        grid.Children.Add(label);
                        Grid.SetRow(label, 1);
                        Grid.SetColumn(label, 2);

                        label = new Label();
                        label.Content = "Entry " + count.ToString() + ": ";
                        grid.Children.Add(label);
                        Grid.SetRow(label, 2);
                        Grid.SetColumn(label, 0);

                        TextBox textBox = new TextBox();
                        textBox.Name = "hash_depends_" + hashDependsList[h] + "_" + count.ToString();
                        textBox.Text = hashDependsList[h];
                        textBox.Margin = new Thickness(4, 0, 4, 0);
                        grid.Children.Add(textBox);
                        Grid.SetRow(textBox, 2);
                        Grid.SetColumn(textBox, 1);

                        textBox = new TextBox();
                        textBox.Name = "hash_depends_flag_" + hashDependsList[h] + "_" + count.ToString();
                        textBox.Text = hashDependsFlagsList[h];
                        textBox.Margin = new Thickness(4, 0, 4, 0);
                        grid.Children.Add(textBox);
                        Grid.SetRow(textBox, 2);
                        Grid.SetColumn(textBox, 2);

                        Button button = new Button();
                        button.Name = "hash_depends_delete_" + hashDependsList[h] + "_" + count.ToString();
                        button.Content = "Remove";
                        button.Click += HashDependsDelete_Click;
                        button.Margin = new Thickness(4, 0, 8, 0);
                        grid.Children.Add(button);
                        Grid.SetRow(button, 2);
                        Grid.SetColumn(button, 3);

                        label = new Label();
                        label.Content = hashDependsNamesList[count];
                        label.FontSize = 12;
                        grid.Children.Add(label);
                        Grid.SetRow(label, 3);
                        Grid.SetColumn(label, 1);
                        Grid.SetColumnSpan(label, 3);

                        MainStackPanel.Children.Add(grid);

                        countActual++;
                    }

                    count++;
                }

                label1.Content = "Hash Depends (" + pageCountStart.ToString() + " to " + (pageCountStart + countActual - 1).ToString() + " out of " + hashDependsList.Count.ToString() + " (0 to " + (hashDependsList.Count - 1).ToString() + ")):";

                label1 = new Label();
                label1.FontSize = 16;
                label1.Content = "Hash Depends (" + pageCountStart.ToString() + " to " + (pageCountStart + countActual - 1).ToString() + " out of " + hashDependsList.Count.ToString() + " (0 to " + (hashDependsList.Count - 1).ToString() + ")):";
                label1.FontWeight = FontWeights.Bold;
                label1.HorizontalAlignment = HorizontalAlignment.Center;
                MainStackPanel.Children.Add(label1);

                if (page == 0)
                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(200.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    if (hashDependsList.Count > 100)
                    {
                        button1 = new Button();
                        button1.Name = "hash_depends_page_forward";
                        button1.Content = ">";
                        button1.FontWeight = FontWeights.Bold;
                        button1.Width = 100.0;
                        button1.Margin = new Thickness(0, 4, 0, 8);
                        button1.Click += HashDependsForward_Click;
                        grid1.Children.Add(button1);
                        Grid.SetRow(button1, 0);
                        Grid.SetColumn(button1, 2);
                    }

                    MainStackPanel.Children.Add(grid1);
                }
                else if (hashDependsList.Count >= pageCountStart && hashDependsList.Count <= pageCountEnd)
                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(200.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    button1 = new Button();
                    button1.Name = "hash_depends_page_back";
                    button1.Content = "<";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 100.0;
                    button1.Margin = new Thickness(0, 4, 0, 8);
                    button1.Click += HashDependsBack_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 0);

                    MainStackPanel.Children.Add(grid1);
                }
                else
                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(100.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    button1 = new Button();
                    button1.Name = "hash_depends_page_back";
                    button1.Content = "<";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 100.0;
                    button1.Margin = new Thickness(0, 4, 0, 8);
                    button1.Click += HashDependsBack_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 0);

                    button1 = new Button();
                    button1.Name = "hash_depends_page_forward";
                    button1.Content = ">";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 100.0;
                    button1.Margin = new Thickness(0, 4, 0, 8);
                    button1.Click += HashDependsForward_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 2);

                    MainStackPanel.Children.Add(grid1);
                }

                {
                    Grid grid1 = new Grid();
                    grid1.HorizontalAlignment = HorizontalAlignment.Center;

                    RowDefinition rowDefinition1 = new RowDefinition();
                    rowDefinition1.Height = new GridLength(1, GridUnitType.Star);
                    grid1.RowDefinitions.Add(rowDefinition1);
                    ColumnDefinition columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(50.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = new GridLength(50.0);
                    grid1.ColumnDefinitions.Add(columnDefinition1);
                    columnDefinition1 = new ColumnDefinition();
                    columnDefinition1.Width = GridLength.Auto;
                    grid1.ColumnDefinitions.Add(columnDefinition1);

                    Label label = new Label();
                    label.Content = "Jump to hash depend number:";
                    grid1.Children.Add(label);
                    Grid.SetRow(label, 1);
                    Grid.SetColumn(label, 0);

                    TextBox textBox = new TextBox();
                    textBox.Name = "hash_depends_jump_to_2";
                    textBox.Text = "";
                    textBox.Margin = new Thickness(4, 0, 4, 0);
                    grid1.Children.Add(textBox);
                    Grid.SetRow(textBox, 1);
                    Grid.SetColumn(textBox, 1);

                    if (MainStackPanel.FindName("hash_depends_jump_to_2") == null)
                    {
                        RegisterName(textBox.Name, textBox);
                    }
                    else
                    {
                        UnregisterName(textBox.Name);

                        RegisterName(textBox.Name, textBox);
                    }

                    button1 = new Button();
                    button1.Name = "hash_depends_jump_2";
                    button1.Content = "Jump";
                    button1.FontWeight = FontWeights.Bold;
                    button1.Width = 60.0;
                    button1.Margin = new Thickness(4, 0, 4, 0);
                    button1.Click += HashDependsJump_Click;
                    grid1.Children.Add(button1);
                    Grid.SetRow(button1, 0);
                    Grid.SetColumn(button1, 2);

                    MainStackPanel.Children.Add(grid1);
                }
            }
        }

        private void HashDependsJump_Click(object sender, RoutedEventArgs e)
        {
            string jumpValueString = "";

            if ((sender as Button).Name.EndsWith("1"))
            {
                var item = (TextBox)MainStackPanel.FindName("hash_depends_jump_to_1");
                jumpValueString = item.Text;
            }
            else
            {
                var item = (TextBox)MainStackPanel.FindName("hash_depends_jump_to_2");
                jumpValueString = item.Text;
            }

            string legalCharacters = "0123456789";

            bool isLegal = true;

            foreach (char a in jumpValueString)
            {
                if (isLegal)
                {
                    bool charIsLegal = false;

                    foreach (char b in legalCharacters)
                    {
                        if (a == b)
                        {
                            charIsLegal = true;
                        }
                    }

                    if (!charIsLegal)
                    {
                        isLegal = false;
                    }
                }
            }

            if (isLegal)
            {
                int jumpValue = 0;

                int.TryParse(jumpValueString, out jumpValue);

                if (jumpValue >= 0 && jumpValue < hashDependsList.Count)
                {
                    int hashDependsNumber = 0;
                    int hashDependsFlagsNumber = 0;

                    bool backup_rpkg_file = false;

                    bool hashDependsCountFound = false;

                    int hashOffset = 0;

                    UpdateHashDependsChanges(ref hashDependsNumber, ref hashDependsFlagsNumber, ref backup_rpkg_file, ref hashDependsCountFound, ref hashOffset);

                    int jumpToPage = jumpValue / 100;

                    LoadHashDependsEditor(jumpToPage);
                }
                else
                {
                    MessageBoxShow("Error: Jump to value is out of range.");

                    return;
                }
            }
            else
            {
                MessageBoxShow("Error: Invalid jump to value.");

                return;
            }
        }

        private void HashDependsForward_Click(object sender, RoutedEventArgs e)
        {
            int hashDependsNumber = 0;
            int hashDependsFlagsNumber = 0;

            bool backup_rpkg_file = false;

            bool hashDependsCountFound = false;

            int hashOffset = 0;

            UpdateHashDependsChanges(ref hashDependsNumber, ref hashDependsFlagsNumber, ref backup_rpkg_file, ref hashDependsCountFound, ref hashOffset);

            LoadHashDependsEditor(hashDependsPage + 1);
        }

        private void HashDependsBack_Click(object sender, RoutedEventArgs e)
        {
            int hashDependsNumber = 0;
            int hashDependsFlagsNumber = 0;

            bool backup_rpkg_file = false;

            bool hashDependsCountFound = false;

            int hashOffset = 0;

            UpdateHashDependsChanges(ref hashDependsNumber, ref hashDependsFlagsNumber, ref backup_rpkg_file, ref hashDependsCountFound, ref hashOffset);
            LoadHashDependsEditor(hashDependsPage - 1);
        }

        private void HashDependsAdd_Click(object sender, RoutedEventArgs e)
        {
            int count = hashDependsList.Count;

            hashDependsList.Add("00123456789ABDEF");

            hashDependsNamesList.Add("New");

            hashDependsFlagsList.Add("00");

            int jumpToPage = count / 100;

            LoadHashDependsEditor(jumpToPage);

            return;
        }

        private void HashDependsDelete_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as Button).Name == "hash_depends_delete_all")
            {
                hashDependsList.Clear();

                hashDependsNamesList.Clear();

                hashDependsFlagsList.Clear();

                LoadHashDependsEditor(0);
            }
            else
            {
                //MessageBoxShow((sender as Button).Name);

                string[] hashDependsToDeleteData = (sender as Button).Name.Split('_');

                int itemToDelete = 0;

                int.TryParse(hashDependsToDeleteData[4], out itemToDelete);

                hashDependsList.RemoveAt(itemToDelete);

                hashDependsNamesList.RemoveAt(itemToDelete);

                hashDependsFlagsList.RemoveAt(itemToDelete);

                LoadHashDependsEditor(hashDependsPage);
            }
        }

        void UpdateHashDependsChanges(ref int hashDependsNumber, ref int hashDependsFlagsNumber, ref bool backup_rpkg_file, ref bool hashDependsCountFound, ref int hashOffset)
        {
            foreach (var child in MainStackPanel.Children)
            {
                if ((child as Label) != null)
                {
                    if (!hashDependsCountFound)
                    {
                        //MessageBoxShow("Label: " + (child as Label).Content);

                        if ((child as Label).Content.ToString().StartsWith("Hash Depends ("))
                        {
                            string[] headerData = (child as Label).Content.ToString().Split(' ');

                            int.TryParse(headerData[2].Replace("(", ""), out hashOffset);

                            hashDependsCountFound = true;
                        }
                        else if ((child as Label).Content.ToString().StartsWith("No Hash Depends"))
                        {
                            hashDependsCountFound = true;
                        }
                    }
                }
            }

            if (hashDependsCountFound)
            {
                hashDependsNumber = hashOffset;
                hashDependsFlagsNumber = hashOffset;
            }
            else
            {
                MessageBoxShow("Error: Could not find / parse the hash offset value.");

                return;
            }

            if (currentHashFileName == "")
            {
                MessageBoxShow("Error: Could not identify the current hash file name.");

                return;
            }

            foreach (var child in MainStackPanel.Children)
            {
                if ((child as Label) != null)
                {
                    if (!hashDependsCountFound)
                    {
                        //MessageBoxShow("Label: " + (child as Label).Content);

                        if ((child as Label).Content.ToString().StartsWith("Hash Depends ("))
                        {
                            string[] headerData = (child as Label).Content.ToString().Split(' ');

                            int.TryParse(headerData[2].Replace("(", ""), out hashOffset);


                        }
                    }
                }
                else if ((child as Grid) != null)
                {
                    foreach (var gridChild in (child as Grid).Children)
                    {
                        if ((gridChild as Label) != null)
                        {
                            //MessageBoxShow("Grid Label: " + (gridChild as Label).Content);
                        }
                        else if ((gridChild as TextBox) != null)
                        {
                            if ((gridChild as TextBox).Name.ToLower().Contains("jump"))
                            {

                            }
                            else if ((gridChild as TextBox).Name.ToLower().Contains("flag"))
                            {
                                //MessageBoxShow("Grid TextBox Name: " + (gridChild as TextBox).Name + ", " + "Grid TextBox Name: " + (gridChild as TextBox).Text);

                                hashDependsFlagsList[hashDependsFlagsNumber] = (gridChild as TextBox).Text;

                                hashDependsFlagsNumber++;

                                if (!IsLegalHashFlag((gridChild as TextBox).Text))
                                {
                                    MessageBoxShow("Error: " + (gridChild as TextBox).Text + " is not a valid hash flag value. Needs to be 2 characters long and consist of only 0-9 and A-F.");

                                    return;
                                }
                            }
                            else
                            {
                                //MessageBoxShow("Grid TextBox Name: " + (gridChild as TextBox).Name + ", " + "Grid TextBox Name: " + (gridChild as TextBox).Text);

                                hashDependsList[hashDependsNumber] = (gridChild as TextBox).Text;

                                hashDependsNumber++;

                                if (!IsLegalHashName((gridChild as TextBox).Text))
                                {
                                    MessageBoxShow("Error: " + (gridChild as TextBox).Text + " is not a valid hash value. Needs to be 16 characters long and consist of only 0-9 and A-F.");

                                    return;
                                }
                            }
                        }
                    }
                }
                else if ((child as ToggleSwitch) != null)
                {
                    //MessageBoxShow("ToggleSwitch: " + (child as ToggleSwitch).IsOn.ToString());

                    if ((child as ToggleSwitch).IsOn)
                    {
                        backup_rpkg_file = true;
                    }
                }
            }
        }

        private void SaveChangesToHashDependsButton_Click(object sender, RoutedEventArgs e)
        {
            int hashDependsNumber = 0;
            int hashDependsFlagsNumber = 0;

            string hashList = "";
            string hashFlagList = "";

            bool backup_rpkg_file = false;

            bool hashDependsCountFound = false;

            int hashOffset = 0;

            UpdateHashDependsChanges(ref hashDependsNumber, ref hashDependsFlagsNumber, ref backup_rpkg_file, ref hashDependsCountFound, ref hashOffset);

            /*if (hashDependsNumber != hashDependsList.Count)
            {
                MessageBoxShow("Error: Parsing the hash depends textboxes for the hash depends hash values.");

                return;
            }

            if (hashDependsFlagsNumber != hashDependsFlagsList.Count)
            {
                MessageBoxShow("Error: Parsing the hash depends flags textboxes for the hash depends hash values.");

                return;
            }*/

            if (hashDependsNumber != hashDependsFlagsNumber)
            {
                MessageBoxShow("Error: The parsed hash depends and hash depends flags are mismatched.");

                return;
            }

            foreach (string tempHashString in hashDependsList)
            {
                hashList += tempHashString;
            }

            foreach (string tempHashFlagString in hashDependsFlagsList)
            {
                hashFlagList += tempHashFlagString;
            }

            MessageQuestion messageBox = new MessageQuestion();

            if (backup_rpkg_file)
            {
                messageBox.message.Content = "Are you sure you want to modify the hash depends of " + currentHashFileName + " and regenerate the RPKG file?\n\nRPKG file: " + hashDependsRPKGFilePath + "\n\nThe original hash depends size: " + hashDependsOriginalList.Count.ToString() + "\n\nThe new hash depends size: " + hashDependsList.Count.ToString() + "\n\nThe backup RPKG toggle option was enabled, so the existing RPKG file will be backed up.";
            }
            else
            {
                messageBox.message.Content = "Are you sure you want to modify the hash depends of " + currentHashFileName + " and regenerate the RPKG file?\n\nRPKG file: " + hashDependsRPKGFilePath + "\n\nThe original hash depends size: " + hashDependsOriginalList.Count.ToString() + "\n\nThe new hash depends size: " + hashDependsList.Count.ToString() + "\n\nThe backup RPKG toggle option was not enabled, so the existing RPKG file will not be backed up.";
            }

            messageBox.ShowDialog();

            if (messageBox.buttonPressed == "OKButton")
            {
                int return_value = 0;

                if (backup_rpkg_file)
                {
                    return_value = modify_hash_depends(hashDependsRPKGFilePath, currentHashFileName, hashList, hashFlagList, (UInt32)hashDependsList.Count, 1);
                }
                else
                {
                    return_value = modify_hash_depends(hashDependsRPKGFilePath, currentHashFileName, hashList, hashFlagList, (UInt32)hashDependsList.Count, 0);
                }

                if (return_value == 0)
                {
                    MessageBoxShow("Hash Depends has been successfully updated for RPKG: " + hashDependsRPKGFilePath);

                    return_value = unload_rpkg(hashDependsRPKGFilePath);

                    SearchRPKGsTreeView.Items.Clear();

                    bool treeview_item_found = false;

                    int count = 0;

                    int treeview_item_index = 0;

                    foreach (TreeViewItem item in MainTreeView.Items)
                    {
                        if (item.Header.ToString() == hashDependsRPKGFilePath)
                        {
                            treeview_item_index = count;

                            treeview_item_found = true;
                        }

                        count++;
                    }

                    if (treeview_item_found)
                    {
                        MainTreeView.Items.RemoveAt(treeview_item_index);

                        ImportRPKGFile(hashDependsRPKGFilePath);

                        foreach (TreeViewItem item in MainTreeView.Items)
                        {
                            if (item.Header.ToString() == hashDependsRPKGFilePath)
                            {
                                item.IsSelected = true;
                            }
                        }
                    }
                    else
                    {
                        MessageBoxShow("Error: Cound not find the treeview item for unloading for RPKG: " + hashDependsRPKGFilePath);
                    }
                }
                else if (return_value == 1)
                {
                    MessageBoxShow("Error: Patch list from RPKG<->RPKG.DLL does not match for RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 2)
                {
                    MessageBoxShow("Error: Hash file was not found in RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 3)
                {
                    MessageBoxShow("Error: RPKG version error for RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 4)
                {
                    MessageBoxShow("Error: Can not open file RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 5)
                {
                    MessageBoxShow("Error: Can not create temporary file for RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 6)
                {
                    MessageBoxShow("Error: Could not delete the existing RPKG file: " + hashDependsRPKGFilePath);
                }
            }
            else if (messageBox.buttonPressed == "CancelButton")
            {
                return;
            }
        }

        private void DiscardChangesToHashDependsButton_Click(object sender, RoutedEventArgs e)
        {
            DetailsTextBox.Visibility = Visibility.Visible;
            MainScrollViewer.Visibility = Visibility.Collapsed;
            MainStackPanel.Visibility = Visibility.Collapsed;
            EditHashDependsButton.Visibility = Visibility.Visible;
            SaveChangesToHashDependsButton.Visibility = Visibility.Collapsed;
            DiscardChangesToHashDependsButton.Visibility = Visibility.Collapsed;
            ExtractHashDependsButton.Visibility = Visibility.Visible;
        }

        private void ExtractHashDependsButton_Click(object sender, RoutedEventArgs e)
        {
            string[] buttons = { "Extract All Hash Depends", "Extract All Hash Depends With PRIM Models (GLB/TGA)", "Extract Direct Hash Depends", "Extract Direct Hash Depends With PRIM Models (GLB/TGA)", "Cancel" };

            RightClickMenu rightClickMenu = new RightClickMenu(buttons);

            rightClickMenu.Left = PointToScreen(Mouse.GetPosition(null)).X;
            rightClickMenu.Top = PointToScreen(Mouse.GetPosition(null)).Y;

            rightClickMenu.ShowDialog();

            string hashValue = DetailsTextBox.Text.Substring(0, DetailsTextBox.Text.IndexOf(' '));

            if (hashValue.Length == 16)
            {
                Progress progress = new Progress();

                progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

                if (rightClickMenu.buttonPressed == "button0")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform all hash depends extraction.");

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button1")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform all hash depends PRIM models extraction.");

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button2")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform direct hash depends extraction.");

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button3")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform direct hash depends PRIM models extraction.");

                        return;
                    }
                }
                else
                {
                    return;
                }

                List<string> rpkgFiles = new List<string>();

                foreach (var filePath in Directory.GetFiles(runtimeDirectory))
                {
                    if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                    {
                        rpkgFiles.Add(filePath);
                    }
                }

                rpkgFiles.Sort(new NaturalStringComparer());

                foreach (string filePath in rpkgFiles)
                {
                    ImportRPKGFile(filePath);
                }

                string command = "";
                string input_path = runtimeDirectory;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = userSettings.OutputFolder;

                if (rightClickMenu.buttonPressed == "button0")
                {
                    command = "-extract_all_hash_depends_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + rightClickedOnName + " To GLB/TGA File(s)...";

                    string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = reset_task_status();

                    execute_task temp_rpkgExecute = task_execute;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button1")
                {
                    command = "-extract_all_hash_depends_prim_models_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + rightClickedOnName + " To GLB/TGA File(s)...";

                    string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = reset_task_status();

                    execute_task temp_rpkgExecute = ExtractMODEL;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button2")
                {
                    command = "-extract_direct_hash_depends_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + rightClickedOnName + " To GLB/TGA File(s)...";

                    string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = reset_task_status();

                    execute_task temp_rpkgExecute = task_execute;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button3")
                {
                    command = "-extract_direct_hash_depends_prim_models_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + rightClickedOnName + " To GLB/TGA File(s)...";

                    string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + rightClickedOnName + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = reset_task_status();

                    execute_task temp_rpkgExecute = ExtractMODEL;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //MessageBoxShow(progress.task_status_string);

                        return;
                    }
                }
            }
            else
            {
                MessageBoxShow("Error: Could not parse the hash value from the details textbox.");
            }
        }
    }
}

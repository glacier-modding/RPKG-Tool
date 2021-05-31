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

                            if (!detailsTextBoxText.ContainsKey(identifier))
                            {
                                DetailsTextBox.Text += hash + " in RPKG file " + rpkgHashFilePath + ":\n";

                                DetailsTextBox.Text += Marshal.PtrToStringAnsi(get_hash_details(rpkgHashFilePath, hash));

                                DetailsTextBox.Text += "\n\n";

                                HexViewerTextBox.Text = "Hex view of " + header[0] + ":\n\n";

                                LocalizationTextBox.Text = "";

                                unsafe
                                {
                                    HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgHashFilePath, hash));
                                }

                                if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                                {
                                    UInt32 localization_data_size = generate_localization_string(rpkgHashFilePath, hash, resourceType);

                                    unsafe
                                    {
                                        byte[] localization_data = new byte[localization_data_size];

                                        Marshal.Copy(get_localization_string(), localization_data, 0, (int)localization_data_size);

                                        if (localization_data_size > 0)
                                        {
                                            LocalizationTextBox.Text = Encoding.UTF8.GetString(localization_data);
                                        }
                                    }

                                    if (localization_data_size > 0)
                                    {
                                        if (ThirdTabRight.Visibility == Visibility.Hidden)
                                        {
                                            ThirdTabRight.Visibility = Visibility.Visible;
                                        }
                                    }
                                    else
                                    {
                                        if (ThirdTabRight.Visibility == Visibility.Visible)
                                        {
                                            ThirdTabRight.Visibility = Visibility.Hidden;
                                        }
                                    }

                                    localizationTextBoxText.Add(identifier, LocalizationTextBox.Text);
                                }
                                else
                                {
                                    if (ThirdTabRight.Visibility == Visibility.Visible)
                                    {
                                        ThirdTabRight.Visibility = Visibility.Hidden;
                                    }
                                }

                                int return_value = clear_hash_data_vector();

                                detailsTextBoxText.Add(identifier, DetailsTextBox.Text);
                                hexViewerTextBoxText.Add(identifier, HexViewerTextBox.Text);
                            }
                            else
                            {
                                string details = "";
                                detailsTextBoxText.TryGetValue(identifier, out details);
                                DetailsTextBox.Text = details;

                                string hex = "";
                                hexViewerTextBoxText.TryGetValue(identifier, out hex);
                                HexViewerTextBox.Text = hex;

                                LocalizationTextBox.Text = "";

                                if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                                {
                                    string localization = "";
                                    localizationTextBoxText.TryGetValue(identifier, out localization);
                                    LocalizationTextBox.Text = localization;
                                }
                            }
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
                    }
                    else if (itemHeader.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
                    {
                        string rpkgFile = itemHeader.Substring(itemHeader.LastIndexOf("\\") + 1);

                        UInt32 allHashesInRPKGCount = get_all_hashes_in_rpkg_count(itemHeader);

                        UInt64 allHashesInRPKGDataSize = get_all_hashes_in_rpkg_data_size(itemHeader);

                        DetailsTextBox.Text = rpkgFile + " has " + allHashesInRPKGCount.ToString("N0") + " hash files/resources,\n";
                        DetailsTextBox.Text += "having a combined total data size of " + allHashesInRPKGDataSize.ToString("N0") + " bytes";
                        DetailsTextBox.Text += Marshal.PtrToStringAnsi(get_patch_deletion_list(itemHeader));

                        HexViewerTextBox.Text = "";
                    }
                    else
                    {
                        string[] header = itemHeader.Split(' ');

                        string hashFile = header[0];

                        string[] hashDetails = hashFile.Split('.');

                        string hash = hashDetails[0];
                        string resourceType = hashDetails[1];

                        string identifier = rpkgFilePath + hash;

                        if (!detailsTextBoxText.ContainsKey(identifier))
                        {
                            DetailsTextBox.Text = hash + " in RPKG file " + rpkgFilePath + ":\n";

                            DetailsTextBox.Text += Marshal.PtrToStringAnsi(get_hash_details(rpkgFilePath, hash));

                            HexViewerTextBox.Text = "Hex view of " + header[0] + ":\n\n";

                            LocalizationTextBox.Text = "";

                            unsafe
                            {
                                HexViewerTextBox.Text += Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash));
                            }

                            if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                            {
                                UInt32 localization_data_size = generate_localization_string(rpkgFilePath, hash, resourceType);

                                unsafe
                                {
                                    byte[] localization_data = new byte[localization_data_size];

                                    Marshal.Copy(get_localization_string(), localization_data, 0, (int)localization_data_size);

                                    if (localization_data_size > 0)
                                    {
                                        LocalizationTextBox.Text = Encoding.UTF8.GetString(localization_data);
                                    }
                                }

                                if (localization_data_size > 0)
                                {
                                    if (ThirdTabRight.Visibility == Visibility.Hidden)
                                    {
                                        ThirdTabRight.Visibility = Visibility.Visible;
                                    }
                                }
                                else
                                {
                                    if (ThirdTabRight.Visibility == Visibility.Visible)
                                    {
                                        ThirdTabRight.Visibility = Visibility.Hidden;
                                    }
                                }

                                localizationTextBoxText.Add(identifier, LocalizationTextBox.Text);
                            }
                            else
                            {
                                if (ThirdTabRight.Visibility == Visibility.Visible)
                                {
                                    ThirdTabRight.Visibility = Visibility.Hidden;
                                }
                            }

                            int return_value = clear_hash_data_vector();

                            detailsTextBoxText.Add(identifier, DetailsTextBox.Text);
                            hexViewerTextBoxText.Add(identifier, HexViewerTextBox.Text);
                        }
                        else
                        {
                            string details = "";
                            detailsTextBoxText.TryGetValue(identifier, out details);
                            DetailsTextBox.Text = details;

                            string hex = "";
                            hexViewerTextBoxText.TryGetValue(identifier, out hex);
                            HexViewerTextBox.Text = hex;

                            LocalizationTextBox.Text = "";

                            if (resourceType == "LOCR" || resourceType == "DLGE" || resourceType == "RTLV")
                            {
                                string localization = "";
                                localizationTextBoxText.TryGetValue(identifier, out localization);
                                LocalizationTextBox.Text = localization;
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

            unsafe
            {
                Marshal.Copy(get_hash_in_rpkg_data(rpkgFilePath, hash), hash_data, 0, (int)hash_size);
            }

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

            if ((MainTreeView.Items.GetItemAt(0) as TreeViewItem).Header.ToString() == "Click")
            {
                MainTreeView.Items.Clear();
            }

            var item = new TreeViewItem();

            item.Header = rpkgFilePath;

            MainTreeView.Items.Add(item);

            Thread.Sleep(200);

            List<string> resourceTypes = new List<string>();

            UInt32 resourceTypeCount = get_resource_types_count(rpkgFilePath);

            for (UInt32 i = 0; i < resourceTypeCount; i++)
            {
                unsafe
                {
                    resourceTypes.Add(Marshal.PtrToStringAnsi(get_resource_types_at(rpkgFilePath, i)));
                }
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

            fileDialog.InitialDirectory = userSettings.InputFolder;

            fileDialog.FileName = userSettings.InputFolder;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                ImportRPKGFile(fileDialog.FileName);

                LoadHashDependsMap();
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

                    unsafe
                    {
                        hash = Marshal.PtrToStringAnsi(get_hash_based_on_resource_type_at(rpkgFilePath, resourceType, i));
                    }

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

                    string[] buttons = { "Extract All", "Cancel" };

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

                    if (rightClickedOnName == "LOCR" || rightClickedOnName == "DLGE" || rightClickedOnName == "RTLV")
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

                    if (hashType == "WWEM" || hashType == "WWES" || hashType == "WWEV")
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
                    else if (rightClickMenu.buttonPressed == "button1" && buttonCount == 3)
                    {
                        if (hashType == "WWEM")
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

                LoadHashDependsMap();
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
                LoadHashDependsMap();
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

            filterDialog.ShowDialog();

            filter = filterDialog.filterString;

            if (massExtractName == "ORES")
            {
                command = "-extract_ores_from";

                progress.ProgressBar.IsIndeterminate = true;

                progress.message.Content = "Extracting ORES To IOI Paths...";
            }
            else if (massExtractName == "WWEM")
            {
                command = "-extract_wwem_to_ogg_from";

                progress.message.Content = "Extracting WWEM To IOI Paths...";
            }
            else if (massExtractName == "WWES")
            {
                command = "-extract_wwes_to_ogg_from";

                progress.message.Content = "Extracting WWES To IOI Paths...";
            }
            else if (massExtractName == "WWEV")
            {
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

        private void Rebuild_Click(object sender, RoutedEventArgs e)
        {
            string rebuildButton = (sender as MenuItem).Header.ToString();

            string rebuildType = "";

            if (rebuildButton.Contains("DLGE"))
            {
                rebuildType = "DLGE";
            }
            else if (rebuildButton.Contains("LOCR"))
            {
                rebuildType = "LOCR";
            }
            else if (rebuildButton.Contains("RTLV"))
            {
                rebuildType = "RTLV";
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

                if (rebuildType == "DLGE")
                {
                    command = "-rebuild_dlge_from_json_from";

                    progress.message.Content = "Rebuilding All DLGE from JSON in " + input_path + "...";
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
                    LoadHashDependsMap();
                }
            }
        }

        private void GenerateRPKGFromFolder_Click(object sender, RoutedEventArgs e)
        {
            string inputFolder = SelectFolder("input", "Select Folder To Generate RPKG From:");

            if (inputFolder != "")
            {
                string outputFolder = SelectFolder("output", "Select Output Folder To Save Generated RPKG To:");

                foreach (var filePath in Directory.GetFiles(inputFolder))
                {
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

                    progress.message.Content = "Importing RPKG file " + rpkgFile + "...";

                    progress.ShowDialog();
                }
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

        private Dictionary<string, string> detailsTextBoxText = new Dictionary<string, string>();
        private Dictionary<string, string> hexViewerTextBoxText = new Dictionary<string, string>();
        private Dictionary<string, string> localizationTextBoxText = new Dictionary<string, string>();
        private UserSettings userSettings;
        public string rpkgFilePath = "";
        private bool rightClickedOnFirst = true;
        private int rightClickedOn = 0;
        private string rightClickedOnName = "";
        private string rightClickedOnFullName = "";
        private bool oneOrMoreRPKGsHaveBeenImported = false;
        private System.Windows.Threading.DispatcherTimer searchRPKGsInputTimer;
        private System.Windows.Threading.DispatcherTimer searchHashListInputTimer;

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
        public static extern  UInt32 get_resource_types_count(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_resource_types_at(string rpkg_file, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_data_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 get_resource_types_data_size(string rpkg_file, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_based_on_resource_type_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_hash_based_on_resource_type_count(string rpkg_file, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_based_on_resource_type_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_hash_based_on_resource_type_at(string rpkg_file, string resource_type, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_all_hashes_in_rpkg_count(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_all_hashes_in_rpkg_at(string rpkg_file, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_details", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_hash_details(string rpkg_file, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_hash_in_rpkg_size(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_hash_in_rpkg_data(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_data_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 get_all_hashes_in_rpkg_data_size(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "clear_hash_data_vector", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_hash_data_vector();

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_data_in_hex_view", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_hash_in_rpkg_data_in_hex_view(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "generate_localization_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe UInt32 generate_localization_string(string rpkg_file_name, string hash_string, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_localization_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_localization_string();

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_hashes_with_no_reverse_depends();

        [DllImport("rpkg.dll", EntryPoint = "load_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int load_hash_list();

        [DllImport("rpkg.dll", EntryPoint = "get_hash_list_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_list_string(string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int get_hashes_with_no_reverse_depends(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_hashes_with_no_reverse_depends_string();

        [DllImport("rpkg.dll", EntryPoint = "get_direct_hash_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int get_direct_hash_depends(string rpkg_file, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_direct_hash_depends_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr get_direct_hash_depends_string();

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
    }
}

using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using MahApps.Metro.Controls;
using rpkg.Extensions;
using rpkg.Utils;

namespace rpkg.Controls
{
    /// <summary>
    /// Interaction logic for ItemDetails.xaml
    /// </summary>
    public partial class ItemDetails : UserControl
    {
        public List<string> patchHashOriginalList;
        public List<string> patchHashList;
        public List<string> patchHashNamesList;
        public List<string> hashDependsOriginalList;
        public List<string> hashDependsList;
        public List<string> hashDependsNamesList;
        public List<string> hashDependsFlagsList;
        public int hashDependsPage;
        private string rpkgFilePath;
        private string resourcePath;
        private string currentHashFileName;
        private string hashDependsRPKGFilePath;

        public ItemDetails()
        {
            InitializeComponent();
        }

        public void ShowRpkgDetails(string rpkgPath)
        {
            string rpkgFile = rpkgPath.Substring(rpkgPath.LastIndexOf("\\") + 1);

            rpkgFilePath = rpkgPath;

            uint allHashesInRPKGCount = RpkgLib.get_all_hashes_in_rpkg_count(rpkgPath);

            ulong allHashesInRPKGDataSize = RpkgLib.get_all_hashes_in_rpkg_data_size(rpkgPath);

            DetailsTextBox.Text = rpkgFile + " has " + allHashesInRPKGCount.ToString("N0") + " hash files/resources,\n";
            DetailsTextBox.Text += "having a combined total data size of " + allHashesInRPKGDataSize.ToString("N0") + " bytes";

            string patchDataString = Marshal.PtrToStringAnsi(RpkgLib.get_patch_deletion_list(rpkgPath));

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
        }

        public void ShowResourceTypeDetails(string rpkgPath, string resourceType)
        {
            rpkgFilePath = rpkgPath;

            uint hashBasedOnResourceTypeCount = RpkgLib.get_hash_based_on_resource_type_count(rpkgPath, resourceType);

            ulong resourceTypesDataSize = RpkgLib.get_resource_types_data_size(rpkgPath, resourceType);

            DetailsTextBox.Text = "Found " + hashBasedOnResourceTypeCount.ToString("N0") + " hash files/resources of resource type " + resourceType + ",\n";
            DetailsTextBox.Text += "having a combined total data size of " + resourceTypesDataSize.ToString("N0") + " bytes";

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

        public void ShowResourceDetails(string rpkgPath, string resourcePath)
        {
            rpkgFilePath = rpkgPath;
            resourcePath = resourcePath;

            string[] header = resourcePath.Split(' ');

            string hashFile = header[0];

            string[] hashDetails = hashFile.Split('.');

            string hash = hashDetails[0];


            currentHashFileName = hashFile;

            hashDependsRPKGFilePath = rpkgFilePath;

            DetailsTextBox.Text = hash + " in RPKG file " + rpkgPath + ":\n";

            DetailsTextBox.Text += Marshal.PtrToStringAnsi(RpkgLib.get_hash_details(rpkgPath, hash));
            
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
        }

        public void ShowSearchResultDetails(string resourceHash, string[] containedRpkgs)
        {
            DetailsTextBox.Text = "";
            
            foreach (string rpkgHashFilePath in containedRpkgs)
            {

                DetailsTextBox.Text += resourceHash + " in RPKG file " + rpkgHashFilePath + ":\n";

                DetailsTextBox.Text += Marshal.PtrToStringAnsi(RpkgLib.get_hash_details(rpkgHashFilePath, resourceHash));

                DetailsTextBox.Text += "\n\n";
            }
        }

        public void ShowNoSearchResults()
        {
            DetailsTextBox.Text = "Hash is not located in any of the currently imported RPKGs.";
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

                    //WindowUtils.MessageBoxShow(patchData[3]);
                }

                if (line.Contains("Patch file has"))
                {
                    string[] patchData = line.Split(' ');

                    int.TryParse(patchData[3], out patchNumber);

                    if (patchNumber > 0)
                    {
                        patchListFound = true;

                        //WindowUtils.MessageBoxShow(patchNumber.ToString());
                    }
                }
            }

            if (patchNumber != patchHashList.Count)
            {
                WindowUtils.MessageBoxShow("Error: Parsing the Details textbox for the patch hash values.");

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
                label.Content = "Entry " + count + ": ";
                grid.Children.Add(label);
                Grid.SetRow(label, 1);
                Grid.SetColumn(label, 0);

                label = new Label();
                label.Content = patchHashNamesList[count];
                label.FontSize = 12;
                MainStackPanel.Children.Add(label);

                TextBox textBox = new TextBox();
                textBox.Name = "patch_" + patchHash + "_" + count;
                textBox.Text = patchHash;
                textBox.Margin = new Thickness(4, 0, 8, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);

                Button button = new Button();
                button.Name = "patch_delete_" + patchHash + "_" + count;
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
            label.Content = "Entry " + count + ": ";
            grid.Children.Add(label);
            Grid.SetRow(label, 1);
            Grid.SetColumn(label, 0);

            label = new Label();
            label.Content = patchHashNamesList[count];
            label.FontSize = 12;
            MainStackPanel.Children.Add(label);

            TextBox textBox = new TextBox();
            textBox.Name = "patch_" + patchHashList[count] + "_" + count;
            textBox.Text = patchHashList[count];
            textBox.Margin = new Thickness(4, 0, 8, 0);
            grid.Children.Add(textBox);
            Grid.SetRow(textBox, 1);
            Grid.SetColumn(textBox, 1);

            Button button = new Button();
            button.Name = "patch_delete_" + patchHashList[count] + "_" + count;
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
                //WindowUtils.MessageBoxShow((sender as Button).Name);

                string[] patchToDeleteData = (sender as Button).Name.Split('_');

                int itemToDelete = 0;

                int.TryParse(patchToDeleteData[3], out itemToDelete);

                patchHashList.RemoveAt(itemToDelete);

                patchHashNamesList.RemoveAt(itemToDelete);

                LoadPatchDeletionEditor();
            }
        }


        private void SaveChangesToPatchDeletionListButton_Click(object sender, RoutedEventArgs e)
        {
            uint patchNumber = 0;

            string patchList = "";

            bool backup_rpkg_file = false;

            foreach (var child in MainStackPanel.Children)
            {
                if ((child as Label) != null)
                {
                    //WindowUtils.MessageBoxShow("Label: " + (child as Label).Content);
                }
                else if ((child as Grid) != null)
                {
                    foreach (var gridChild in (child as Grid).Children)
                    {
                        if ((gridChild as Label) != null)
                        {
                            //WindowUtils.MessageBoxShow("Grid Label: " + (gridChild as Label).Content);
                        }
                        else if ((gridChild as TextBox) != null)
                        {
                            //WindowUtils.MessageBoxShow("Grid TextBox Name: " + (gridChild as TextBox).Name + ", " + "Grid TextBox Name: " + (gridChild as TextBox).Text);

                            patchNumber++;

                            patchList += (gridChild as TextBox).Text;

                            if (!HashUtils.IsLegalHashName((gridChild as TextBox).Text))
                            {
                                WindowUtils.MessageBoxShow("Error: " + (gridChild as TextBox).Text + " is not a valid hash value. Needs to be 16 characters long and consist of only 0-9 and A-F.");

                                return;
                            }
                        }
                    }
                }
                else if ((child as ToggleSwitch) != null)
                {
                    //WindowUtils.MessageBoxShow("ToggleSwitch: " + (child as ToggleSwitch).IsOn.ToString());

                    if ((child as ToggleSwitch).IsOn)
                    {
                        backup_rpkg_file = true;
                    }
                }
            }

            if (patchNumber != patchHashList.Count)
            {
                WindowUtils.MessageBoxShow("Error: Parsing the patch deletion textboxes for the patch hash values.");

                return;
            }

            MessageQuestion messageBox = new MessageQuestion();

            if (backup_rpkg_file)
            {
                messageBox.message.Content = "Are you sure you want to modify the patch deletion list and regenerate the RPKG file?\n\nRPKG file: " + rpkgFilePath + "\n\nThe original patch deletion list size: " + patchHashOriginalList.Count + "\n\nThe new patch deletion list size: " + patchNumber + "\n\nThe backup RPKG toggle option was enabled, so the existing RPKG file will be backed up.";
            }
            else
            {
                messageBox.message.Content = "Are you sure you want to modify the patch deletion list and regenerate the RPKG file?\n\nRPKG file: " + rpkgFilePath + "\n\nThe original patch deletion list size: " + patchHashOriginalList.Count + "\n\nThe new patch deletion list size: " + patchNumber + "\n\nThe backup RPKG toggle option was not enabled, so the existing RPKG file will not be backed up.";
            }

            messageBox.ShowDialog();

            if (messageBox.buttonPressed == "OKButton")
            {
                int return_value = 0;

                if (backup_rpkg_file)
                {
                    return_value = RpkgLib.modify_patch_deletion_list(rpkgFilePath, patchList, patchNumber, 1);
                }
                else
                {
                    return_value = RpkgLib.modify_patch_deletion_list(rpkgFilePath, patchList, patchNumber, 0);
                }

                if (return_value == 0)
                {
                    WindowUtils.MessageBoxShow("Patch deletion list has been successfully updated for RPKG: " + rpkgFilePath);
                    RpkgManager.ReloadRpkg(rpkgFilePath);
                }
                else if (return_value == 1)
                {
                    WindowUtils.MessageBoxShow("Error: Patch list from RPKG<->RPKG.DLL does not match for RPKG: " + rpkgFilePath);
                }
                else if (return_value == 2)
                {
                    WindowUtils.MessageBoxShow("Error: RPKG is not a patch file: " + rpkgFilePath);
                }
                else if (return_value == 3)
                {
                    WindowUtils.MessageBoxShow("Error: RPKG version error for RPKG: " + rpkgFilePath);
                }
                else if (return_value == 4)
                {
                    WindowUtils.MessageBoxShow("Error: Can not open file RPKG: " + rpkgFilePath);
                }
                else if (return_value == 5)
                {
                    WindowUtils.MessageBoxShow("Error: Can not create temporary file for RPKG: " + rpkgFilePath);
                }
                else if (return_value == 6)
                {
                    WindowUtils.MessageBoxShow("Error: Could not delete the existing RPKG file: " + rpkgFilePath);
                }
            }
            else if (messageBox.buttonPressed == "CancelButton")
            {
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

                        //WindowUtils.MessageBoxShow(hashDependsData[3]);
                    }
                }

                if (line.StartsWith("Depends on"))
                {
                    string[] hashDependsData = line.Split(' ');

                    int.TryParse(hashDependsData[2], out hashDependsNumber);

                    if (hashDependsNumber > 0)
                    {
                        hashDependsFound = true;

                        //WindowUtils.MessageBoxShow(hashDependsNumber.ToString());
                    }
                }
            }

            if (hashDependsNumber != hashDependsList.Count)
            {
                WindowUtils.MessageBoxShow("Error: Parsing the Details textbox for the hash depends values.");

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
                        label.Content = "Entry " + count + ": ";
                        grid.Children.Add(label);
                        Grid.SetRow(label, 2);
                        Grid.SetColumn(label, 0);

                        TextBox textBox = new TextBox();
                        textBox.Name = "hash_depends_" + hashDependsList[h] + "_" + count;
                        textBox.Text = hashDependsList[h];
                        textBox.Margin = new Thickness(4, 0, 4, 0);
                        grid.Children.Add(textBox);
                        Grid.SetRow(textBox, 2);
                        Grid.SetColumn(textBox, 1);

                        textBox = new TextBox();
                        textBox.Name = "hash_depends_flag_" + hashDependsList[h] + "_" + count;
                        textBox.Text = hashDependsFlagsList[h];
                        textBox.Margin = new Thickness(4, 0, 4, 0);
                        grid.Children.Add(textBox);
                        Grid.SetRow(textBox, 2);
                        Grid.SetColumn(textBox, 2);

                        Button button = new Button();
                        button.Name = "hash_depends_delete_" + hashDependsList[h] + "_" + count;
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

                label1.Content = "Hash Depends (" + pageCountStart + " to " + (pageCountStart + countActual - 1) + " out of " + hashDependsList.Count + " (0 to " + (hashDependsList.Count - 1) + ")):";

                label1 = new Label();
                label1.FontSize = 16;
                label1.Content = "Hash Depends (" + pageCountStart + " to " + (pageCountStart + countActual - 1) + " out of " + hashDependsList.Count + " (0 to " + (hashDependsList.Count - 1) + ")):";
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
                    WindowUtils.MessageBoxShow("Error: Jump to value is out of range.");
                }
            }
            else
            {
                WindowUtils.MessageBoxShow("Error: Invalid jump to value.");
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
                //WindowUtils.MessageBoxShow((sender as Button).Name);

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
                        //WindowUtils.MessageBoxShow("Label: " + (child as Label).Content);

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
                WindowUtils.MessageBoxShow("Error: Could not find / parse the hash offset value.");

                return;
            }

            if (currentHashFileName == "")
            {
                WindowUtils.MessageBoxShow("Error: Could not identify the current hash file name.");

                return;
            }

            foreach (var child in MainStackPanel.Children)
            {
                if ((child as Label) != null)
                {
                    if (!hashDependsCountFound)
                    {
                        //WindowUtils.MessageBoxShow("Label: " + (child as Label).Content);

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
                            //WindowUtils.MessageBoxShow("Grid Label: " + (gridChild as Label).Content);
                        }
                        else if ((gridChild as TextBox) != null)
                        {
                            if ((gridChild as TextBox).Name.ToLower().Contains("jump"))
                            {

                            }
                            else if ((gridChild as TextBox).Name.ToLower().Contains("flag"))
                            {
                                //WindowUtils.MessageBoxShow("Grid TextBox Name: " + (gridChild as TextBox).Name + ", " + "Grid TextBox Name: " + (gridChild as TextBox).Text);

                                hashDependsFlagsList[hashDependsFlagsNumber] = (gridChild as TextBox).Text;

                                hashDependsFlagsNumber++;

                                if (!HashUtils.IsLegalHashFlag((gridChild as TextBox).Text))
                                {
                                    WindowUtils.MessageBoxShow("Error: " + (gridChild as TextBox).Text + " is not a valid hash flag value. Needs to be 2 characters long and consist of only 0-9 and A-F.");

                                    return;
                                }
                            }
                            else
                            {
                                //WindowUtils.MessageBoxShow("Grid TextBox Name: " + (gridChild as TextBox).Name + ", " + "Grid TextBox Name: " + (gridChild as TextBox).Text);

                                hashDependsList[hashDependsNumber] = (gridChild as TextBox).Text;

                                hashDependsNumber++;

                                if (!HashUtils.IsLegalHashName((gridChild as TextBox).Text))
                                {
                                    WindowUtils.MessageBoxShow("Error: " + (gridChild as TextBox).Text + " is not a valid hash value. Needs to be 16 characters long and consist of only 0-9 and A-F.");

                                    return;
                                }
                            }
                        }
                    }
                }
                else if ((child as ToggleSwitch) != null)
                {
                    //WindowUtils.MessageBoxShow("ToggleSwitch: " + (child as ToggleSwitch).IsOn.ToString());

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
                WindowUtils.MessageBoxShow("Error: Parsing the hash depends textboxes for the hash depends hash values.");

                return;
            }

            if (hashDependsFlagsNumber != hashDependsFlagsList.Count)
            {
                WindowUtils.MessageBoxShow("Error: Parsing the hash depends flags textboxes for the hash depends hash values.");

                return;
            }*/

            if (hashDependsNumber != hashDependsFlagsNumber)
            {
                WindowUtils.MessageBoxShow("Error: The parsed hash depends and hash depends flags are mismatched.");

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
                messageBox.message.Content = "Are you sure you want to modify the hash depends of " + currentHashFileName + " and regenerate the RPKG file?\n\nRPKG file: " + hashDependsRPKGFilePath + "\n\nThe original hash depends size: " + hashDependsOriginalList.Count + "\n\nThe new hash depends size: " + hashDependsList.Count + "\n\nThe backup RPKG toggle option was enabled, so the existing RPKG file will be backed up.";
            }
            else
            {
                messageBox.message.Content = "Are you sure you want to modify the hash depends of " + currentHashFileName + " and regenerate the RPKG file?\n\nRPKG file: " + hashDependsRPKGFilePath + "\n\nThe original hash depends size: " + hashDependsOriginalList.Count + "\n\nThe new hash depends size: " + hashDependsList.Count + "\n\nThe backup RPKG toggle option was not enabled, so the existing RPKG file will not be backed up.";
            }

            messageBox.ShowDialog();

            if (messageBox.buttonPressed == "OKButton")
            {
                int return_value = 0;

                if (backup_rpkg_file)
                {
                    return_value = RpkgLib.modify_hash_depends(hashDependsRPKGFilePath, currentHashFileName, hashList, hashFlagList, (uint)hashDependsList.Count, 1);
                }
                else
                {
                    return_value = RpkgLib.modify_hash_depends(hashDependsRPKGFilePath, currentHashFileName, hashList, hashFlagList, (uint)hashDependsList.Count, 0);
                }

                if (return_value == 0)
                {
                    WindowUtils.MessageBoxShow("Hash Depends has been successfully updated for RPKG: " + hashDependsRPKGFilePath);
                    RpkgManager.ReloadRpkg(rpkgFilePath);
                }
                else if (return_value == 1)
                {
                    WindowUtils.MessageBoxShow("Error: Patch list from RPKG<->RPKG.DLL does not match for RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 2)
                {
                    WindowUtils.MessageBoxShow("Error: Hash file was not found in RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 3)
                {
                    WindowUtils.MessageBoxShow("Error: RPKG version error for RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 4)
                {
                    WindowUtils.MessageBoxShow("Error: Can not open file RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 5)
                {
                    WindowUtils.MessageBoxShow("Error: Can not create temporary file for RPKG: " + hashDependsRPKGFilePath);
                }
                else if (return_value == 6)
                {
                    WindowUtils.MessageBoxShow("Error: Could not delete the existing RPKG file: " + hashDependsRPKGFilePath);
                }
            }
            else if (messageBox.buttonPressed == "CancelButton")
            {
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

            string[] nodeData = resourcePath.Split(' ');

            if (hashValue.Length == 16)
            {
                Progress progress = new Progress();

                progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

                if (rightClickMenu.buttonPressed == "button0")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        WindowUtils.MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform all hash depends extraction.");

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button1")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        WindowUtils.MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform all hash depends PRIM models extraction.");

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button2")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        WindowUtils.MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform direct hash depends extraction.");

                        return;
                    }
                }
                else if (rightClickMenu.buttonPressed == "button3")
                {
                    if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
                    {
                        WindowUtils.MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform direct hash depends PRIM models extraction.");

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

                rpkgFiles = rpkgFiles.SortNaturally();
                RpkgManager.LoadRpkgs(rpkgFiles);

                string command = "";
                string input_path = runtimeDirectory;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = App.Settings.OutputFolder;

                if (rightClickMenu.buttonPressed == "button0")
                {
                    command = "-extract_all_hash_depends_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

                    string temp_outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task temp_rpkgExecute = RpkgLib.task_execute;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //WindowUtils.MessageBoxShow(progress.task_status_string);
                    }
                }
                else if (rightClickMenu.buttonPressed == "button1")
                {
                    command = "-extract_all_hash_depends_prim_models_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

                    string temp_outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task temp_rpkgExecute = MainWindow.ExtractMODEL;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //WindowUtils.MessageBoxShow(progress.task_status_string);
                    }
                }
                else if (rightClickMenu.buttonPressed == "button2")
                {
                    command = "-extract_direct_hash_depends_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

                    string temp_outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task temp_rpkgExecute = RpkgLib.task_execute;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //WindowUtils.MessageBoxShow(progress.task_status_string);
                    }
                }
                else if (rightClickMenu.buttonPressed == "button3")
                {
                    command = "-extract_direct_hash_depends_prim_models_from";

                    progress.operation = (int)Progress.Operation.MASS_EXTRACT;

                    filter = hashValue;

                    progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

                    string temp_outputFolder = WindowUtils.SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:");

                    if (temp_outputFolder == "")
                    {
                        return;
                    }

                    output_path = temp_outputFolder;

                    int temp_return_value = RpkgLib.reset_task_status();

                    RpkgLib.execute_task temp_rpkgExecute = MainWindow.ExtractMODEL;

                    IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                    progress.ShowDialog();

                    if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
                    {
                        //WindowUtils.MessageBoxShow(progress.task_status_string);
                    }
                }
            }
            else
            {
                WindowUtils.MessageBoxShow("Error: Could not parse the hash value from the details textbox.");
            }
        }

    }
}

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

namespace rpkg
{
    /// <summary>
    /// Interaction logic for EntityBrickEditor.xaml
    /// </summary>
    public partial class EntityBrickEditor : MetroWindow
    {
        public EntityBrickEditor()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            EventManager.RegisterClassHandler(typeof(TreeViewItem), TreeViewItem.MouseLeftButtonUpEvent, new RoutedEventHandler(TreeViewItem_MouseLeftButtonUpEvent));

            LoadMainTreeView();
        }

        private void MetroWindow_Activated(object sender, EventArgs e)
        {
            if (hidden)
            {
                LoadMainTreeView();
                hidden = false;
            }
        }

        private void LoadMainTreeView()
        {
            //clear_temp_tblu_data();

            MainTreeView.Items.Clear();

            while (MainStackPanel.Children.Count > 0)
            {
                MainStackPanel.Children.RemoveAt(MainStackPanel.Children.Count - 1);
            }

            controlNames = null;

            controlNames = new List<string>();

            controlCount = 0;

            {
                var topItem = new TreeViewItem();

                topItem.Header = tempFileNameFull.Replace("(", "").Replace(")", "");

                string responseString = Marshal.PtrToStringAnsi(get_top_level_logical_parents(temps_index));

                //MessageBoxShow(responseString);

                string[] topLevelParents = responseString.Trim(',').Split(',');

                foreach (string topLevelParent in topLevelParents)
                {
                    UInt32 logical_parent = 0xFFFFFFFF;

                    UInt32.TryParse(topLevelParent, out logical_parent);

                    responseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(temps_index, logical_parent));

                    if (responseString != "")
                    {
                        string[] topLevelEntries = responseString.Trim(',').Split(',');

                        foreach (string entry in topLevelEntries)
                        {
                            string[] entryData = entry.Split('|');
                            string entityName = entryData[1];
                            UInt32 entryIndex = 0;

                            UInt32.TryParse(entryData[0], out entryIndex);

                            var item = new TreeViewItem();

                            item.Header = entityName + " (" + entryData[0] + ") (" + temps_index.ToString() + ")";

                            item.Expanded += Item_Expanded;

                            item.Items.Add(null);

                            //LoadTreeView(entryIndex, entityName, ref item);

                            topItem.Items.Add(item);
                        }
                    }
                }

                MainTreeView.Items.Add(topItem);
            }

            {
                string responseString = Marshal.PtrToStringAnsi(get_all_bricks(temps_index));

                //MessageBoxShow(responseString);

                if (responseString != "")
                {
                    string[] bricks = responseString.Trim(',').Split(',');

                    foreach (string brick in bricks)
                    {
                        string[] brickData = brick.Split(' ');

                        //MessageBoxShow(brick);

                        int temp_index_hash_reference = get_temp_index(brickData[0]);

                        //MessageBoxShow(temp_index_hash_reference.ToString());

                        var topItem = new TreeViewItem();

                        topItem.Header = brick;

                        responseString = Marshal.PtrToStringAnsi(get_top_level_logical_parents((UInt32)temp_index_hash_reference));

                        //MessageBoxShow(responseString);

                        string[] topLevelParents = responseString.Trim(',').Split(',');

                        foreach (string topLevelParent in topLevelParents)
                        {
                            UInt32 logical_parent = 0xFFFFFFFF;

                            UInt32.TryParse(topLevelParent, out logical_parent);

                            responseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent((UInt32)temp_index_hash_reference, logical_parent));

                            if (responseString != "")
                            {
                                string[] topLevelEntries = responseString.Trim(',').Split(',');

                                foreach (string entry in topLevelEntries)
                                {
                                    string[] entryData = entry.Split('|');
                                    string entityName = entryData[1];
                                    UInt32 entryIndex = 0;

                                    UInt32.TryParse(entryData[0], out entryIndex);

                                    var item = new TreeViewItem();

                                    item.Header = entityName + " (" + entryData[0] + ") (" + temp_index_hash_reference.ToString() + ")";

                                    item.Expanded += Item_Expanded;

                                    item.Items.Add(null);

                                    //LoadTreeView(entryIndex, entityName, ref item);

                                    topItem.Items.Add(item);
                                }
                            }
                        }

                        MainTreeView.Items.Add(topItem);
                    }
                }
            }

            /*UInt32 logical_parent = 0xFFFFFFFF;

            responseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(temps_index, logical_parent));

            if (responseString != "")
            {
                string[] topLevelEntries = responseString.Trim(',').Split(',');

                foreach (string entry in topLevelEntries)
                {
                    string[] entryData = entry.Split('|');
                    string entityName = entryData[1];
                    UInt32 entryIndex = 0;

                    UInt32.TryParse(entryData[0], out entryIndex);

                    var item = new TreeViewItem();

                    item.Header = entityName + " (" + entryData[0] + ") (" + temps_index.ToString() + ")";

                    item.Expanded += Item_Expanded;

                    item.Items.Add(null);

                    //LoadTreeView(entryIndex, entityName, ref item);

                    MainTreeView.Items.Add(item);
                }
            }

            logical_parent = 0xFFFFFFFE;

            responseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(temps_index, logical_parent));

            if (responseString != "")
            {
                string[] topLevelEntries = responseString.Trim(',').Split(',');

                foreach (string entry in topLevelEntries)
                {
                    string[] entryData = entry.Split('|');
                    string entityName = entryData[1];
                    UInt32 entryIndex = 0;

                    UInt32.TryParse(entryData[0], out entryIndex);

                    var item = new TreeViewItem();

                    item.Header = entityName + " (" + entryData[0] + ") (" + temps_index.ToString() + ")";

                    item.Expanded += Item_Expanded;

                    item.Items.Add(null);

                    //LoadTreeView(entryIndex, entityName, ref item);

                    MainTreeView.Items.Add(item);
                }
            }*/
        }

        private void UpdateTempFile()
        {
            if (controlNames != null)
            {
                foreach (string controlName in controlNames)
                {
                    DependencyObject descendant = FindDescendant(EditorWindow, controlName);

                    string[] controlData = controlName.Split('_');

                    if (descendant != null)
                    {
                        if (descendant is TextBox)
                        {
                            string value = (descendant as TextBox).Text;

                            //MessageBoxShow("TextBox: " + controlName + ", Type: " + controlData[1] + ", Shared: " + controlData[2] + ", Temp Index: " + controlData[3] + ", EntityIndex: " + controlData[4] + ", Index: " + controlData[5] + ", offset:" + controlData[6] + ", Value: " + value);

                            if (value != "")
                            {
                                UInt32 temp_index = 0;

                                UInt32.TryParse(controlData[3], out temp_index);

                                if (controlData[2] == "s")
                                {
                                    string[] valueData = value.Replace("(offset=0x", "").Replace(")", "").Split(' ');

                                    //MessageBoxShow("update_temp_file_pointer(" + controlData[3] + ", " + controlData[4] + ", " + valueData[1]);

                                    int return_value = update_temp_file_pointer(temp_index, controlData[4], controlData[5], valueData[1]);
                                }
                                else
                                {
                                    //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + value);

                                    if (controlData[1] == "enum")
                                    {
                                        string[] valueData = value.Replace("offset=0x", "").Replace("(", "").Replace(")", "").Split(' ');

                                        //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + valueData[1]);

                                        int return_value = update_temp_file(temp_index, controlData[6], controlData[1], valueData[1]);
                                    }
                                    else
                                    {
                                        int return_value = update_temp_file(temp_index, controlData[6], controlData[1], value);
                                    }                                    
                                }
                            }
                        }
                        else if (descendant is ComboBox)
                        {
                            string value = (descendant as ComboBox).SelectedItem.ToString();

                            //MessageBoxShow("ComboBox: " + controlName + ", Type: " + controlData[1] + ", Shared: " + controlData[2] + ", Temp Index: " + controlData[3] + ", EntityIndex: " + controlData[4] + ", Index: " + controlData[5] + ", offset:" + controlData[6] + ", Value: " + value);

                            if (value != "")
                            {
                                UInt32 temp_index = 0;

                                UInt32.TryParse(controlData[3], out temp_index);

                                if (controlData[2] == "s")
                                {
                                    string[] valueData = value.Replace("(offset=0x", "").Replace(")", "").Split(' ');

                                    //MessageBoxShow("update_temp_file_pointer(" + controlData[3] + ", " + controlData[4] + ", " + valueData[1]);

                                    //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + valueData[1]);

                                    int return_value = update_temp_file_pointer(temp_index, controlData[4], controlData[5], valueData[1]);
                                }
                                else
                                {
                                    //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + value);

                                    if (controlData[1] == "enum")
                                    {
                                        string[] valueData = value.Replace("offset=0x", "").Replace("(", "").Replace(")", "").Split(' ');

                                        //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + valueData[1]);
                                        
                                        int return_value = update_temp_file(temp_index, controlData[6], controlData[1], valueData[1]);
                                    }
                                    else
                                    {
                                        int return_value = update_temp_file(temp_index, controlData[6], controlData[1], value);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        private void TreeViewItem_MouseLeftButtonUpEvent(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            if (!hidden)
            {
                TreeViewItem item = (sender as TreeViewItem);

                item.IsSelected = true;

                string header = item.Header.ToString();

                if (MainTreeView.Items.Count > 0)
                {
                    UpdateTempFile();

                    //MessageBoxShow(header);

                    while (MainStackPanel.Children.Count > 0)
                    {
                        MainStackPanel.Children.RemoveAt(MainStackPanel.Children.Count - 1);
                    }

                    controlNames = null;

                    controlNames = new List<string>();

                    controlCount = 0;

                    if ((item.Header as string).Contains("("))
                    {
                        string[] headerData = (item.Header as string).Replace("(", "").Replace(")", "").Split(' ');

                        string entityName = "";

                        for (int i = 0; i < (headerData.Length - 2); i++)
                        {
                            entityName += headerData[i];

                            if (i != (headerData.Length - 3))
                            {
                                entityName += " ";
                            }
                        }

                        UInt32 entryIndex = 0;

                        UInt32.TryParse(headerData[headerData.Length - 2], out entryIndex);

                        UInt32 temp_index = 0;

                        UInt32.TryParse(headerData[headerData.Length - 1], out temp_index);

                        string entryIndexString = headerData[headerData.Length - 2];

                        //MessageBoxShow(entryIndexString);
                        //MessageBoxShow(temp_index.ToString());
                        //MessageBoxShow(entityName);

                        string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_reference_data(temp_index, entryIndex));

                        string responseString = Marshal.PtrToStringAnsi(get_entries_data(temp_index, entryIndex));

                        //MessageBoxShow(hashReferenceData);

                        //MessageBoxShow(responseString);

                        Label label1 = new Label();
                        label1.Content = entityName + "'s Data:";
                        label1.FontSize = 18;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanel.Children.Add(label1);

                        label1 = new Label();
                        label1.Content = "Hash reference data:";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanel.Children.Add(label1);

                        Label label2 = new Label();
                        label2.Content = hashReferenceData;

                        MainStackPanel.Children.Add(label2);

                        label1 = new Label();
                        label1.Content = "Property data:";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanel.Children.Add(label1);

                        if (responseString == "")
                        {
                            label2 = new Label();
                            label2.Content = "  None";

                            MainStackPanel.Children.Add(label2);
                        }
                        else
                        {
                            string[] entryDataStrings = responseString.Trim(',').Split(',');

                            foreach (string entryDataString in entryDataStrings)
                            {
                                if (entryDataString != "")
                                {
                                    string[] entryData = entryDataString.Split('|');

                                    UInt32 propertyCRC32 = 0;
                                    UInt32.TryParse(entryData[0], out propertyCRC32);

                                    string propertyCRC32String = entryData[1];

                                    if (propertyCRC32String == "")
                                    {
                                        propertyCRC32String = "Unknown (CRC32=" + entryData[0] + ")";
                                    }

                                    string propertyType = entryData[2];

                                    string propertyTypeIndex = entryData[3];

                                    string propertyTypeIndexIndex = entryData[4];

                                    UInt32 propertyOffset = 0;
                                    UInt32.TryParse(entryData[5], out propertyOffset);

                                    string logicalParent = entryData[6];

                                    string propertyData = entryData[7];

                                    //MessageBoxShow(entryDataString);

                                    //MessageBoxShow(propertyData);

                                    responseString = Marshal.PtrToStringAnsi(get_enum_values(temp_index, propertyType));

                                    //MessageBoxShow(responseString);

                                    if (responseString != "")
                                    {
                                        AppendInput_enum(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex, responseString);
                                    }
                                    else if (propertyType == "bool")
                                    {
                                        AppendInput_bool(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "uint8")
                                    {
                                        AppendInput_uint8(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "int32")
                                    {
                                        AppendInput_int32(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "uint32")
                                    {
                                        AppendInput_uint32(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "float32")
                                    {
                                        AppendInput_float32(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "SMatrix43")
                                    {
                                        AppendInput_SMatrix43(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex, logicalParent);
                                    }
                                    else if (propertyType == "SVector2")
                                    {
                                        AppendInput_SVector2(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "SVector3")
                                    {
                                        AppendInput_SVector3(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "SVector4")
                                    {
                                        AppendInput_SVector4(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "SColorRGB")
                                    {
                                        AppendInput_SColorRGB(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "ZGuid")
                                    {
                                        AppendInput_ZGuid(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else if (propertyType == "ZString")
                                    {
                                        AppendInput_ZString(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                    else
                                    {
                                        AppendInput_NotImplementedYet(temp_index, entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyType, propertyTypeIndex, propertyTypeIndexIndex);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        private void AppendInput_enum(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex, string enum_values)
        {
            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                //shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
            }

            int enumInt = 0;

            int.TryParse(propertyData, out enumInt);

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (enum) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (enum) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            ComboBox comboBox = new ComboBox();
                        
            comboBox.Name = GetNewControlName("ComboBox_enum_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

            string[] valuesStrings = enum_values.Trim(',').Split(',');

            int index = 0;
            int indexCount = 0;

            foreach (string valueString in valuesStrings)
            {
                string[] valueData = valueString.Split('|');

                UInt32 enumNum = 0;
                UInt32.TryParse(valueData[0], out enumNum);

                if (enumInt == enumNum)
                {
                    index = indexCount;
                }
                
                indexCount++;

                comboBox.Items.Add(valueData[1] + " (" + valueData[0] + ") (offset=0x" + propertyOffset.ToString("X") + ")");
            }

            comboBox.SelectedIndex = index;

            comboBox.Margin = new Thickness(4, 0, 4, 0);
            grid.Children.Add(comboBox);
            Grid.SetRow(comboBox, 1);
            Grid.SetColumn(comboBox, 1);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_NotImplementedYet(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyType, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            Label label1 = new Label();

            label1.Content = propertyCRC32String + " (" + propertyType + ") (offset=0x" + propertyOffset.ToString("X") + "):";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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

            Label label = new Label();
            label.Content = "  - Resource display/parsing for this type has not implemented yet.";
            grid.Children.Add(label);
            Grid.SetRow(label, 1);
            Grid.SetColumn(label, 0);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_ZString(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            Label label1 = new Label();

            label1.Content = propertyCRC32String + " (ZString) (offset=0x" + propertyOffset.ToString("X") + "):";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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

            Label label = new Label();
            label.Content = "ZString:";
            grid.Children.Add(label);
            Grid.SetRow(label, 1);
            Grid.SetColumn(label, 0);

            TextBox textBox = new TextBox();
            textBox.IsReadOnly = true;
            textBox.ToolTip = "Set to read only until a future update when entity/brick rebuilding is working.";
            textBox.Name = GetNewControlName("TextBox_ZString_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
            controlNames.Add(textBox.Name);
            textBox.Text = propertyData.Replace('@', ',');
            textBox.Margin = new Thickness(4, 0, 4, 0);
            grid.Children.Add(textBox);
            Grid.SetRow(textBox, 1);
            Grid.SetColumn(textBox, 1);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_ZGuid(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            Label label1 = new Label();

            label1.Content = propertyCRC32String + " (ZGuid) (offset=0x" + propertyOffset.ToString("X") + "):";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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

            Label label = new Label();
            label.Content = "ZGuid:";
            grid.Children.Add(label);
            Grid.SetRow(label, 1);
            Grid.SetColumn(label, 0);

            TextBox textBox = new TextBox();
            textBox.IsReadOnly = true;
            textBox.ToolTip = "Set to read only until a future update when entity/brick rebuilding is working.";
            textBox.Name = GetNewControlName("TextBox_ZGuid_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
            controlNames.Add(textBox.Name);
            textBox.Text = propertyData;
            textBox.Margin = new Thickness(4, 0, 4, 0);
            grid.Children.Add(textBox);
            Grid.SetRow(textBox, 1);
            Grid.SetColumn(textBox, 1);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_bool(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(temp_index, propertyOffset.ToString(), propertyTypeIndex));
            }

            int boolInt = 0;

            int.TryParse(propertyData, out boolInt);

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (bool) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (bool) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            ComboBox comboBox = new ComboBox();

            if (isShared)
            {
                comboBox.Name = GetNewControlName("ComboBox_bool_s_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

                string[] valuesStrings = shared_values.Trim(',').Split(',');

                foreach (string valueString in valuesStrings)
                {
                    string[] valueData = valueString.Split('|');

                    UInt32 offset = 0;
                    UInt32.TryParse(valueData[0], out offset);

                    comboBox.Items.Add(valueData[1] + " (offset=0x" + offset.ToString("X") + ")");
                }

                comboBox.SelectedIndex = shared_index;
            }
            else
            {
                comboBox.Name = GetNewControlName("ComboBox_bool_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

                comboBox.Items.Add("True");
                comboBox.Items.Add("False");

                if (boolInt == 0)
                {
                    comboBox.SelectedIndex = 1;
                }
                else
                {
                    comboBox.SelectedIndex = 0;
                }
            }
            
            comboBox.Margin = new Thickness(4, 0, 4, 0);
            grid.Children.Add(comboBox);
            Grid.SetRow(comboBox, 1);
            Grid.SetColumn(comboBox, 1);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_uint8(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(temp_index, propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (uint8) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (uint8) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            if (isShared)
            {
                ComboBox comboBox = new ComboBox();

                comboBox.Name = GetNewControlName("ComboBox_uint8_s_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

                string[] valuesStrings = shared_values.Trim(',').Split(',');

                foreach (string valueString in valuesStrings)
                {
                    string[] valueData = valueString.Split('|');

                    UInt32 offset = 0;
                    UInt32.TryParse(valueData[0], out offset);

                    comboBox.Items.Add(valueData[1] + " (offset=0x" + offset.ToString("X") + ")");
                }

                comboBox.SelectedIndex = shared_index;

                comboBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(comboBox);
                Grid.SetRow(comboBox, 1);
                Grid.SetColumn(comboBox, 1);
            }
            else
            {
                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_uint8_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_int32(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(temp_index, propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (int32) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (int32) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            if (isShared)
            {
                ComboBox comboBox = new ComboBox();

                comboBox.Name = GetNewControlName("ComboBox_int32_s_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

                string[] valuesStrings = shared_values.Trim(',').Split(',');

                foreach (string valueString in valuesStrings)
                {
                    string[] valueData = valueString.Split('|');

                    UInt32 offset = 0;
                    UInt32.TryParse(valueData[0], out offset);

                    comboBox.Items.Add(valueData[1] + " (offset=0x" + offset.ToString("X") + ")");
                }

                comboBox.SelectedIndex = shared_index;

                comboBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(comboBox);
                Grid.SetRow(comboBox, 1);
                Grid.SetColumn(comboBox, 1);
            }
            else
            {
                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_int32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_uint32(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(temp_index, propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (uint32) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (uint32) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            if (isShared)
            {
                ComboBox comboBox = new ComboBox();

                comboBox.Name = GetNewControlName("ComboBox_uint32_s_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

                string[] valuesStrings = shared_values.Trim(',').Split(',');

                foreach (string valueString in valuesStrings)
                {
                    string[] valueData = valueString.Split('|');

                    UInt32 offset = 0;
                    UInt32.TryParse(valueData[0], out offset);

                    comboBox.Items.Add(valueData[1] + " (offset=0x" + offset.ToString("X") + ")");
                }

                comboBox.SelectedIndex = shared_index;

                comboBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(comboBox);
                Grid.SetRow(comboBox, 1);
                Grid.SetColumn(comboBox, 1);
            }
            else
            {
                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_uint32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_float32(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(temp_index, propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (float32) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (float32) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            if (isShared)
            {
                ComboBox comboBox = new ComboBox();

                comboBox.Name = GetNewControlName("ComboBox_float32_s_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

                string[] valuesStrings = shared_values.Trim(',').Split(',');

                foreach (string valueString in valuesStrings)
                {
                    string[] valueData = valueString.Split('|');

                    UInt32 offset = 0;
                    UInt32.TryParse(valueData[0], out offset);

                    comboBox.Items.Add(valueData[1] + " (offset=0x" + offset.ToString("X") + ")");
                }

                comboBox.SelectedIndex = shared_index;

                comboBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(comboBox);
                Grid.SetRow(comboBox, 1);
                Grid.SetColumn(comboBox, 1);
            }
            else
            {
                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }            

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_SMatrix43(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex, string logicalParent)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                //shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (SMatrix43) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + ") (Relative to " + logicalParent + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (SMatrix43) (offset=0x" + propertyOffset.ToString("X") + ") (Relative to " + logicalParent + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            rowDefinition = new RowDefinition();
            rowDefinition.Height = new GridLength(1, GridUnitType.Star);
            grid.RowDefinitions.Add(rowDefinition);
            rowDefinition = new RowDefinition();
            rowDefinition.Height = new GridLength(8);
            grid.RowDefinitions.Add(rowDefinition);
            rowDefinition = new RowDefinition();
            rowDefinition.Height = new GridLength(1, GridUnitType.Star);
            grid.RowDefinitions.Add(rowDefinition);
            rowDefinition = new RowDefinition();
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
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            string[][] labelStrings = { new String[] { "X Axis:", "x:", "y:", "z:" }, new String[] { "Y Axis:", "x:", "y:", "z:" }, new String[] { "Z Axis:", "x:", "y:", "z:" }, new String[] { "Transform:", "x:", "y:", "z:" } };

            int rowCount = 0;

            int textBoxCount = 0;

            foreach (string[] tempLabelStrings in labelStrings)
            {
                int columnCount = 0;

                rowCount++;

                foreach (string labelString in tempLabelStrings)
                {
                    Label label = new Label();
                    label.Content = labelString;
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, columnCount);
                    columnCount++;

                    if (columnCount > 1)
                    {
                        TextBox textBox = new TextBox();
                        textBox.Name = GetNewControlName("TextBox_float32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());                        
                        propertyOffset += 4;
                        textBox.Text = vectorData[textBoxCount];
                        textBox.Margin = new Thickness(4, 0, 4, 0);
                        grid.Children.Add(textBox);
                        Grid.SetRow(textBox, rowCount);
                        Grid.SetColumn(textBox, columnCount);
                        textBoxCount++;
                        columnCount++;
                    }
                }

                rowCount++;
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_SVector2(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                //shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (SVector2) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (SVector2) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            string[] labelStrings = { "x:", "y:" };

            int rowCount = 1;

            int textBoxCount = 0;

            int columnCount = 0;

            foreach (string labelString in labelStrings)
            {
                Label label = new Label();
                label.Content = labelString;
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);
                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                propertyOffset += 4;
                textBox.Text = vectorData[textBoxCount];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);
                textBoxCount++;
                columnCount++;
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_SVector3(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                //shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (SVector3) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (SVector3) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            string[] labelStrings = { "x:", "y:", "z:" };

            int rowCount = 1;

            int textBoxCount = 0;

            int columnCount = 0;

            foreach (string labelString in labelStrings)
            {
                Label label = new Label();
                label.Content = labelString;
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);
                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                propertyOffset += 4;
                textBox.Text = vectorData[textBoxCount];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);
                textBoxCount++;
                columnCount++;
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_SVector4(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                //shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (SVector4) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (SVector4) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            string[] labelStrings = { "w:", "x:", "y:", "z:" };

            int rowCount = 1;

            int textBoxCount = 0;

            int columnCount = 0;

            foreach (string labelString in labelStrings)
            {
                Label label = new Label();
                label.Content = labelString;
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);
                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                propertyOffset += 4;
                textBox.Text = vectorData[textBoxCount];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);
                textBoxCount++;
                columnCount++;
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_SColorRGB(UInt32 temp_index, string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            Random random = new Random();
            int randomNum = random.Next(0, 10000);

            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(temp_index, propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(temp_index, propertyOffset.ToString(), propertyTypeIndex);

                //shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
            }

            Label label1 = new Label();

            if (isShared)
            {
                label1.Content = propertyCRC32String + " (SColorRGB) (shared by " + shared_count + ") (offset=0x" + propertyOffset.ToString("X") + "):";
            }
            else
            {
                label1.Content = propertyCRC32String + " (SColorRGB) (offset=0x" + propertyOffset.ToString("X") + "):";
            }

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanel.Children.Add(label1);

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
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            string[] labelStrings = { "r:", "g:", "b:" };

            int rowCount = 1;

            int textBoxCount = 0;

            int columnCount = 0;

            foreach (string labelString in labelStrings)
            {
                Label label = new Label();
                label.Content = labelString;
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);
                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = "TextBox_float32_ns_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString() + "_" + textBoxCount.ToString() + "_" + randomNum.ToString();
                propertyOffset += 4;
                controlNames.Add(textBox.Name);
                textBox.Text = vectorData[textBoxCount];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);
                textBoxCount++;
                columnCount++;
            }

            propertyOffset -= 4;

            float r = 0;
            float g = 0;
            float b = 0;

            float.TryParse(vectorData[0], out r);
            float.TryParse(vectorData[1], out g);
            float.TryParse(vectorData[2], out b);

            MahApps.Metro.Controls.ColorCanvas colorCanvas = new ColorCanvas();
            colorCanvas.Name = "ColorCanvas_" + temp_index.ToString() + "_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString() + "_" + randomNum.ToString();

            colorCanvas.R = FloatToByte(r);
            colorCanvas.G = FloatToByte(g);
            colorCanvas.B = FloatToByte(b);

            colorCanvas.SelectedColorChanged += ColorCanvas_SelectedColorChanged;

            grid.Children.Add(colorCanvas);
            Grid.SetRow(colorCanvas, 3);
            Grid.SetColumnSpan(colorCanvas, 7);

            //LoadColorPicker(r, g, b);

            MainStackPanel.Children.Add(grid);
        }

        private void ColorCanvas_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            ColorCanvas colorCanvas = (sender as ColorCanvas);

            string[] propertyOffset = colorCanvas.Name.Split('_');

            UInt32 offset = 0;

            UInt32.TryParse(propertyOffset[4], out offset);

            DependencyObject descendant = FindDescendant(EditorWindow, "TextBox_float32_ns_" + propertyOffset[1] + "_" + propertyOffset[2] + "_" + propertyOffset[3] + "_" + offset.ToString() + "_2_" + propertyOffset[5]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.B);

            offset -= 4;

            descendant = FindDescendant(EditorWindow, "TextBox_float32_ns_" + propertyOffset[1] + "_" + propertyOffset[2] + "_" + propertyOffset[3] + "_" + offset.ToString() + "_1_" + propertyOffset[5]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.G);

            offset -= 4;

            descendant = FindDescendant(EditorWindow, "TextBox_float32_ns_" + propertyOffset[1] + "_" + propertyOffset[2] + "_" + propertyOffset[3] + "_" + offset.ToString() + "_0_" + propertyOffset[5]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.R);
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

        private string StringByteToStringFloat(byte input)
        {
            int temp_int = (int)input;

            float temp_float = (float)temp_int / 255.0F;

            return temp_float.ToString();
        }

        public string GetNewControlName(string input)
        {
            input += "_" + controlCount.ToString();

            controlNames.Add(input);

            controlCount++;

            return input;
        }

        private static DependencyObject FindDescendant(DependencyObject parent, string name)
        {
            FrameworkElement element = parent as FrameworkElement;

            if ((element != null) && (element.Name == name))
            {
                return parent;
            }

            int num_children = VisualTreeHelper.GetChildrenCount(parent);

            for (int i = 0; i < num_children; i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(parent, i);
                DependencyObject descendant = FindDescendant(child, name);

                if (descendant != null)
                {
                    return descendant;
                }
            }

            return null;
        }

        private void LoadColorPicker(float r, float g, float b)
        {
            ColorPicker colorPicker = new ColorPicker();

            colorPicker.red = r;
            colorPicker.green = g;
            colorPicker.blue = b;

            colorPicker.ShowDialog();
        }

        private void Item_Expanded(object sender, RoutedEventArgs e)
        {
            var item = (sender as TreeViewItem);

            if (item.Items.Count == 1 && item.Items[0] == null)
            {
                item.Items.Clear();

                string[] header = (item.Header as string).Replace("(", "").Replace(")", "").Split(' ');

                string entityName = "";

                for (int i = 0; i < (header.Length - 2); i++)
                {
                    entityName += header[i];

                    if (i != (header.Length - 3))
                    {
                        entityName += " ";
                    }
                }

                UInt32 entryIndex = 0;

                UInt32.TryParse(header[header.Length - 2], out entryIndex);

                UInt32 temp_index = 0;

                UInt32.TryParse(header[header.Length - 1], out temp_index);

                string responseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(temp_index, entryIndex));

                string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_references(temp_index, entryIndex));

                string[] hashReferences = hashReferenceData.Split(',');

                int temp_index_hash_reference = get_temp_index(hashReferences[0]);

                if (temp_index_hash_reference > 0)
                {
                    UInt32 logical_parent = 0xFFFFFFFF;

                    string responseStringHashReference = Marshal.PtrToStringAnsi(get_entries_with_logical_parent((UInt32)temp_index_hash_reference, logical_parent));

                    if (responseStringHashReference != "")
                    {
                        string[] topLevelEntriesHashReference = responseStringHashReference.Trim(',').Split(',');

                        foreach (string entryHashReference in topLevelEntriesHashReference)
                        {
                            string[] entryDataHashReference = entryHashReference.Split('|');
                            string entityNameHashReference = entryDataHashReference[1];
                            UInt32 entryIndexHashReference = 0;

                            UInt32.TryParse(entryDataHashReference[0], out entryIndexHashReference);

                            var itemHashReference = new TreeViewItem();

                            itemHashReference.Header = entityNameHashReference + " (" + entryDataHashReference[0] + ") (" + temp_index_hash_reference.ToString() + ")";

                            itemHashReference.Expanded += Item_Expanded;

                            itemHashReference.Items.Add(null);

                            //LoadTreeView(entryIndex, entityName, ref item);

                            item.Items.Add(itemHashReference);
                        }
                    }
                }

                //MessageBoxShow(temp_index_hash_reference.ToString());

                string[] topLevelEntries = responseString.Trim(',').Split(',');

                foreach (string entry in topLevelEntries)
                {
                    if (entry != "")
                    {
                        string[] tempEntryData = entry.Split('|');
                        string tempEntityName = tempEntryData[1];
                        UInt32 tempEntryIndex = 0;

                        UInt32.TryParse(tempEntryData[0], out tempEntryIndex);

                        var item2 = new TreeViewItem();

                        item2.Header = tempEntityName + " (" + tempEntryData[0] + ") (" + temp_index.ToString() + ")";

                        item2.Expanded += Item_Expanded;

                        item2.Items.Add(null);

                        item.Items.Add(item2);
                    }
                }
            }
        }

        private void LoadTreeView(UInt32 entryIndex, string entityName, ref TreeViewItem masterTreeViewItem)
        {
            string responseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(temps_index, entryIndex));

            string[] topLevelEntries = responseString.Trim(',').Split(',');

            foreach (string entry in topLevelEntries)
            {
                if (entry != "")
                {
                    string[] tempEntryData = entry.Split('|');
                    string tempEntityName = tempEntryData[1];
                    UInt32 tempEntryIndex = 0;

                    UInt32.TryParse(tempEntryData[0], out tempEntryIndex);

                    var item = new TreeViewItem();

                    item.Header = tempEntityName;

                    LoadTreeView(tempEntryIndex, tempEntityName, ref item);

                    masterTreeViewItem.Items.Add(item);
                }
            }
        }

        private void GenerateTEMPFile_Click(object sender, RoutedEventArgs e)
        {
            UpdateTempFile();

            int changedCount = get_number_of_changed_temps();

            if (changedCount == 0)
            {
                MessageBoxShow("No TEMP files have been changed.");
            }
            else
            {
                int totalCount = get_total_numer_of_temps();

                MessageQuestion messageBox = new MessageQuestion();
                messageBox.message.Content = "A total of " + changedCount.ToString() + " out of " + totalCount.ToString() + " TEMP files have been changed.\n\nWould you like to continue generating TEMP files for them all?";
                messageBox.ShowDialog();

                if (messageBox.buttonPressed == "OKButton")
                {
                    MessageBoxShow("WARNING: When selecting the output folder on the next dialog window...\n\nDo not select the Hitman Runtime folder as the output folder otherwise your Hitman game RPKGs could be overwritten!");

                    var folderDialog = new Ookii.Dialogs.Wpf.VistaFolderBrowserDialog();

                    folderDialog.Description = "Save Changes And Generate New TEMP file(s):";

                    folderDialog.UseDescriptionForTitle = true;

                    if (!System.IO.Directory.Exists(outputFolder))
                    {
                        outputFolder = System.IO.Directory.GetCurrentDirectory();
                    }

                    folderDialog.SelectedPath = outputFolder;

                    var folderDialogResult = folderDialog.ShowDialog();

                    if (folderDialogResult == true)
                    {
                        outputFolder = folderDialog.SelectedPath;

                        outputFolder = outputFolder.Trim('\\') + "\\";

                        int return_value = generate_temp_files_from_data(outputFolder);

                        if (return_value == 0)
                        {
                            MessageBoxShow("TEMP file(s) successfully generated in: " + outputFolder);
                        }
                        else
                        {
                            MessageBoxShow("Error generating TEMP file(s) in: " + outputFolder);
                        }
                    }
                }
                else if (messageBox.buttonPressed == "CancelButton")
                {

                }
            }
        }

        private void GenerateRPKGFile_Click(object sender, RoutedEventArgs e)
        {
            UpdateTempFile();

            int changedCount = get_number_of_changed_temps();

            if (changedCount == 0)
            {
                MessageBoxShow("No TEMP files have been changed.");
            }
            else
            {
                int totalCount = get_total_numer_of_temps();

                MessageQuestion messageBox = new MessageQuestion();
                messageBox.message.Content = "A total of " + changedCount.ToString() + " out of " + totalCount.ToString() + " TEMP files have been changed.\n\nWould you like to continue generating RPKG files with them all?";
                messageBox.ShowDialog();

                if (messageBox.buttonPressed == "OKButton")
                {
                    MessageBoxShow("WARNING: When selecting the output folder on the next dialog window...\n\nDo not select the Hitman Runtime folder as the output folder otherwise your Hitman game RPKGs could be overwritten!");

                    var folderDialog = new Ookii.Dialogs.Wpf.VistaFolderBrowserDialog();

                    folderDialog.Description = "Save Changes And Generate New Patch RPKG File(s) From TEMP File(s):";

                    folderDialog.UseDescriptionForTitle = true;

                    if (!System.IO.Directory.Exists(outputFolder))
                    {
                        outputFolder = System.IO.Directory.GetCurrentDirectory();
                    }

                    folderDialog.SelectedPath = outputFolder;

                    var folderDialogResult = folderDialog.ShowDialog();

                    if (folderDialogResult == true)
                    {
                        outputFolder = folderDialog.SelectedPath;

                        outputFolder = outputFolder.Trim('\\') + "\\";

                        int return_value = generate_rpkg_files_from_data(outputFolder);

                        if (return_value == 0)
                        {
                            MessageBoxShow("TEMP file(s) successfully generated in: " + outputFolder);
                        }
                        else
                        {
                            MessageBoxShow("Error generating TEMP file(s) in: " + outputFolder);
                        }
                    }
                }
                else if (messageBox.buttonPressed == "CancelButton")
                {

                }
            }
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            clear_temp_tblu_data();
            hidden = true;
            this.Hide();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = true;

            clear_temp_tblu_data();
            hidden = true;
            this.Hide();
        }

        private void MessageBoxShow(string messageBoxString)
        {
            Message messageBox = new Message();

            messageBox.message.Content = messageBoxString;

            messageBox.ShowDialog();
        }

        public List<string> matrixStringList;
        public List<string> propertyNamesList;
        public List<string> gltfNamesList;
        public List<string> gltfParentList;
        public List<string> gltfNodeEntryList;
        public List<int> gltfGodotIndexList;
        public List<string> controlNames;
        public int controlCount = 0;
        public UInt32 temps_index = 0;
        public string tempFileName = "";
        public string tbluFileName = "";
        public string rpkgFilePath = "";
        public string tempFileNameFull = "";
        public string inputFolder = "";
        public string outputFolder = "";
        public bool hidden = false;

        enum RPKGStatus
        {
            READY,
            TASK_EXECUTING,
            TASK_SUCCESSFUL,
            TASK_EXITED,
            TASK_RETURNED,
            RPKG_IMPORTING,
            RPKG_IMPORTED,
            RPKG_ALREADY_IMPORTED,
            SCANNING_DIRECTORY,
            SCANNING_DIRECTORY_DONE,
            ABORT_CURRENT_TASK,
            TEMP_TBLU_FOUND,
            TEMP_TBLU_NOT_FOUND_IN_DEPENDS,
            TEMP_TBLU_NOT_FOUND_IN_RPKG,
            TEMP_TBLU_TOO_MANY,
            TEMP_HEADER_NOT_FOUND,
            TEMP_TBLU_ENTRY_COUNT_MISMATCH,
            PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1,
            PRIM_OBJECT_IS_NOT_A_MESH_TYPE,
            TEMP_VERSION_UNKNOWN,
            TBLU_VERSION_UNKNOWN
        };

        struct vector2
        {
            public float x;
            public float y;
        };

        struct vector3
        {
            public float x;
            public float y;
            public float z;
        };

        struct vector4
        {
            public float x;
            public float y;
            public float z;
            public float w;
        };

        struct matrix43
        {
            public vector3 x_axis;
            public vector3 y_axis;
            public vector3 z_axis;
            public vector3 transform;
        };

        public delegate int execute_generate_rpkg_files_from_data(string outputFolder);

        public delegate int execute_export_map_data_to_folder(UInt32 temps_index, string map_name, string output_path);

        public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "task_execute", CallingConvention = CallingConvention.Cdecl)]
        public static extern int task_execute(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "reset_task_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_status();

        [DllImport("rpkg.dll", EntryPoint = "clear_temp_tblu_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_temp_tblu_data();

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent(UInt32 temps_index, UInt32 logical_parent);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_data(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_reference_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_reference_data(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_references", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_references(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "update_temp_file", CallingConvention = CallingConvention.Cdecl)]
        public static extern int update_temp_file(UInt32 temps_index, string offset, string type, string value);

        [DllImport("rpkg.dll", EntryPoint = "generate_temp_file_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_temp_file_from_data(UInt32 temps_index, string temp_file_path);

        [DllImport("rpkg.dll", EntryPoint = "is_offset_shared", CallingConvention = CallingConvention.Cdecl)]
        public static extern int is_offset_shared(UInt32 temps_index, string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_shared_values", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_shared_values(UInt32 temps_index, string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "get_shared_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_shared_index(UInt32 temps_index, string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "get_shared_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_shared_count(UInt32 temps_index, string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "update_temp_file_pointer", CallingConvention = CallingConvention.Cdecl)]
        public static extern int update_temp_file_pointer(UInt32 temps_index, string entry_index, string property_index, string offset);

        [DllImport("rpkg.dll", EntryPoint = "get_enum_values", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_enum_values(UInt32 temps_index, string property_type);

        [DllImport("rpkg.dll", EntryPoint = "get_prim_from_temp", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_prim_from_temp(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_index(string temp_hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_number_of_changed_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_number_of_changed_temps();

        [DllImport("rpkg.dll", EntryPoint = "get_changed_temp_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_changed_temp_data(UInt32 temp_changed_index);

        [DllImport("rpkg.dll", EntryPoint = "generate_temp_files_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_temp_files_from_data(string temp_path);

        [DllImport("rpkg.dll", EntryPoint = "get_total_numer_of_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_total_numer_of_temps();

        [DllImport("rpkg.dll", EntryPoint = "generate_rpkg_files_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_rpkg_files_from_data(string temp_path);

        [DllImport("rpkg.dll", EntryPoint = "get_top_level_logical_parents", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_top_level_logical_parents(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_bricks", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_bricks(UInt32 temps_index);
    }
}

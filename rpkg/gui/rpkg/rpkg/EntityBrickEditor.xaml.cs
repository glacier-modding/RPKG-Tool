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

            UInt32 logical_parent = 0xFFFFFFFF;

            string reponseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(logical_parent));

            if (reponseString != "")
            {
                string[] topLevelEntries = reponseString.Trim(',').Split(',');

                foreach (string entry in topLevelEntries)
                {
                    string[] entryData = entry.Split('|');
                    string entityName = entryData[1];
                    UInt32 entryIndex = 0;

                    UInt32.TryParse(entryData[0], out entryIndex);

                    var item = new TreeViewItem();

                    item.Header = entityName.Replace(" ", "_") + " (" + entryData[0] + ")";

                    item.Expanded += Item_Expanded;

                    item.Items.Add(null);

                    //LoadTreeView(entryIndex, entityName, ref item);

                    MainTreeView.Items.Add(item);
                }
            }
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

                            //MessageBoxShow("TextBox: " + controlName + ", Type: " + controlData[1] + ", Shared: " + controlData[2] + ", EntityIndex: " + controlData[3] + ", Index: " + controlData[4] + ", offset:" + controlData[5] + ", Value: " + value);

                            if (value != "")
                            {
                                if (controlData[2] == "s")
                                {
                                    string[] valueData = value.Replace("(offset=0x", "").Replace(")", "").Split(' ');

                                    //MessageBoxShow("update_temp_file_pointer(" + controlData[3] + ", " + controlData[4] + ", " + valueData[1]);

                                    int return_value = update_temp_file_pointer(controlData[3], controlData[4], valueData[1]);
                                }
                                else
                                {
                                    //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + value);

                                    int return_value = update_temp_file(controlData[5], controlData[1], value);
                                }
                            }
                        }
                        else if (descendant is ComboBox)
                        {
                            string value = (descendant as ComboBox).SelectedItem.ToString();

                            //MessageBoxShow("ComboBox: " + controlName + ", Type: " + controlData[1] + ", Shared: " + controlData[2] + ", EntityIndex: " + controlData[3] + ", Index: " + controlData[4] + ", offset:" + controlData[5] + ", Value: " + value);

                            if (value != "")
                            {
                                if (controlData[2] == "s")
                                {
                                    string[] valueData = value.Replace("(offset=0x", "").Replace(")", "").Split(' ');

                                    //MessageBoxShow("update_temp_file_pointer(" + controlData[3] + ", " + controlData[4] + ", " + valueData[1]);

                                    int return_value = update_temp_file_pointer(controlData[3], controlData[4], valueData[1]);
                                }
                                else
                                {
                                    //MessageBoxShow("update_temp_file(" + controlData[5] + ", " + controlData[1] + ", " + value);

                                    int return_value = update_temp_file(controlData[5], controlData[1], value);
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

                    string[] headerData = (item.Header as string).Replace("(", "").Replace(")", "").Split(' ');

                    string entityName = headerData[0];

                    UInt32 entryIndex = 0;

                    UInt32.TryParse(headerData[1], out entryIndex);

                    string entryIndexString = headerData[1];

                    //MessageBoxShow(headerData[1]);
                    //MessageBoxShow(entryIndex.ToString());

                    string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_reference_data(entryIndex));

                    string reponseString = Marshal.PtrToStringAnsi(get_entries_data(entryIndex));

                    //MessageBoxShow(hashReferenceData);

                    //MessageBoxShow(reponseString);

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

                    if (reponseString == "")
                    {
                        label2 = new Label();
                        label2.Content = "  None";

                        MainStackPanel.Children.Add(label2);
                    }
                    else
                    {
                        string[] entryDataStrings = reponseString.Trim(',').Split(',');

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

                                if (propertyType == "bool")
                                {
                                    AppendInput_bool(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "uint8")
                                {
                                    AppendInput_uint8(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "int32")
                                {
                                    AppendInput_int32(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "uint32")
                                {
                                    AppendInput_uint32(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "float32")
                                {
                                    AppendInput_float32(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "SMatrix43")
                                {
                                    AppendInput_SMatrix43(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex, logicalParent);
                                }
                                else if (propertyType == "SVector2")
                                {
                                    AppendInput_SVector2(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "SVector3")
                                {
                                    AppendInput_SVector3(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "SVector4")
                                {
                                    AppendInput_SVector4(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "SColorRGB")
                                {
                                    AppendInput_SColorRGB(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "ZGuid")
                                {
                                    AppendInput_ZGuid(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else if (propertyType == "ZString")
                                {
                                    AppendInput_ZString(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                                else
                                {
                                    AppendInput_NotImplementedYet(entryIndexString, propertyCRC32String.Replace("_", "__"), propertyData, propertyOffset, propertyType, propertyTypeIndex, propertyTypeIndexIndex);
                                }
                            }
                        }
                    }
                }
            }
        }

        private void AppendInput_NotImplementedYet(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyType, string propertyTypeIndex, string propertyTypeIndexIndex)
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

        private void AppendInput_ZString(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
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
            textBox.Name = GetNewControlName("TextBox_ZString_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
            controlNames.Add(textBox.Name);
            textBox.Text = propertyData;
            textBox.Margin = new Thickness(4, 0, 4, 0);
            grid.Children.Add(textBox);
            Grid.SetRow(textBox, 1);
            Grid.SetColumn(textBox, 1);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_ZGuid(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
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
            textBox.Name = GetNewControlName("TextBox_ZGuid_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
            controlNames.Add(textBox.Name);
            textBox.Text = propertyData;
            textBox.Margin = new Thickness(4, 0, 4, 0);
            grid.Children.Add(textBox);
            Grid.SetRow(textBox, 1);
            Grid.SetColumn(textBox, 1);

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_bool(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
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
                comboBox.Name = GetNewControlName("ComboBox_bool_s_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

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
                comboBox.Name = GetNewControlName("ComboBox_bool_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

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

        private void AppendInput_uint8(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
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

                comboBox.Name = GetNewControlName("ComboBox_uint8_s_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

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
                textBox.Name = GetNewControlName("TextBox_uint8_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_int32(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
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

                comboBox.Name = GetNewControlName("ComboBox_int32_s_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

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
                textBox.Name = GetNewControlName("TextBox_int32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_uint32(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
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

                comboBox.Name = GetNewControlName("ComboBox_uint32_s_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

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
                textBox.Name = GetNewControlName("TextBox_uint32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_float32(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

                shared_values = Marshal.PtrToStringAnsi(get_all_shared_values(propertyOffset.ToString(), propertyTypeIndex));
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

                comboBox.Name = GetNewControlName("ComboBox_float32_s_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());

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
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
                textBox.Text = propertyData;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, 1);
            }            

            MainStackPanel.Children.Add(grid);
        }

        private void AppendInput_SMatrix43(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex, string logicalParent)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

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
                        textBox.Name = GetNewControlName("TextBox_float32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());                        
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

        private void AppendInput_SVector2(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

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
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
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

        private void AppendInput_SVector3(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

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
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
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

        private void AppendInput_SVector4(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

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
                textBox.Name = GetNewControlName("TextBox_float32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString());
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

        private void AppendInput_SColorRGB(string entryIndexString, string propertyCRC32String, string propertyData, UInt32 propertyOffset, string propertyTypeIndex, string propertyTypeIndexIndex)
        {
            Random random = new Random();
            int randomNum = random.Next(0, 10000);

            string[] vectorData = propertyData.Split('!');

            bool isShared = false;

            int return_value = is_offset_shared(propertyOffset.ToString(), propertyTypeIndex);

            string shared_values = "";

            int shared_index = 0;

            int shared_count = 0;

            if (return_value == 1)
            {
                isShared = true;

                shared_index = get_shared_index(propertyOffset.ToString(), propertyTypeIndex);

                shared_count = get_shared_count(propertyOffset.ToString(), propertyTypeIndex);

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
                textBox.Name = "TextBox_float32_ns_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString() + "_" + textBoxCount.ToString() + "_" + randomNum.ToString();
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
            colorCanvas.Name = "ColorCanvas_" + entryIndexString + "_" + propertyTypeIndexIndex + "_" + propertyOffset.ToString() + "_" + randomNum.ToString();

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

            UInt32.TryParse(propertyOffset[3], out offset);

            DependencyObject descendant = FindDescendant(EditorWindow, "TextBox_float32_ns_" + propertyOffset[1] + "_" + propertyOffset[2] + "_" + offset.ToString() + "_2_" + propertyOffset[4]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.B);

            offset -= 4;

            descendant = FindDescendant(EditorWindow, "TextBox_float32_ns_" + propertyOffset[1] + "_" + propertyOffset[2] + "_" + offset.ToString() + "_1_" + propertyOffset[4]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.G);

            offset -= 4;

            descendant = FindDescendant(EditorWindow, "TextBox_float32_ns_" + propertyOffset[1] + "_" + propertyOffset[2] + "_" + offset.ToString() + "_0_" + propertyOffset[4]);
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

                string entityName = header[0];

                UInt32 entryIndex = 0;

                UInt32.TryParse(header[1], out entryIndex);

                string reponseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(entryIndex));

                string[] topLevelEntries = reponseString.Trim(',').Split(',');

                foreach (string entry in topLevelEntries)
                {
                    if (entry != "")
                    {
                        string[] tempEntryData = entry.Split('|');
                        string tempEntityName = tempEntryData[1];
                        UInt32 tempEntryIndex = 0;

                        UInt32.TryParse(tempEntryData[0], out tempEntryIndex);

                        var item2 = new TreeViewItem();

                        item2.Header = tempEntityName.Replace(" ", "_") + " (" + tempEntryData[0] + ")";

                        item2.Expanded += Item_Expanded;

                        item2.Items.Add(null);

                        item.Items.Add(item2);
                    }
                }
            }
        }

        private void LoadTreeView(UInt32 entryIndex, string entityName, ref TreeViewItem masterTreeViewItem)
        {
            string reponseString = Marshal.PtrToStringAnsi(get_entries_with_logical_parent(entryIndex));

            string[] topLevelEntries = reponseString.Trim(',').Split(',');

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

            var fileDialog = new Ookii.Dialogs.Wpf.VistaSaveFileDialog();

            fileDialog.Title = "Save Changes And Generate New TEMP file:";

            fileDialog.Filter = "TEMP file|*.temp|All files|*.*";

            if (!System.IO.Directory.Exists(outputFolder))
            {
                outputFolder = System.IO.Directory.GetCurrentDirectory();
            }

            fileDialog.InitialDirectory = outputFolder;

            fileDialog.FileName = tempFileName;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                int return_value = generate_temp_file_from_data(fileDialog.FileName);

                if (return_value == 0)
                {
                    MessageBoxShow("TEMP file successfully generated: " + fileDialog.FileName);
                }
                else
                {
                    MessageBoxShow("Error generating TEMP file: " + fileDialog.FileName);
                }
            }
        }

        private void GenerateRPKGFile_Click(object sender, RoutedEventArgs e)
        {
            UpdateTempFile();

            var fileDialog = new Ookii.Dialogs.Wpf.VistaSaveFileDialog();

            fileDialog.Title = "Save Changes And Generate New Patch RPKG File From TEMP File:";

            fileDialog.Filter = "RPKG file|*.rpkg|All files|*.*";

            if (!System.IO.Directory.Exists(outputFolder))
            {
                outputFolder = System.IO.Directory.GetCurrentDirectory();
            }

            fileDialog.InitialDirectory = outputFolder;

            string rpkgFile = rpkgFilePath.Substring(rpkgFilePath.LastIndexOf("\\") + 1);

            fileDialog.FileName = "generated_" + rpkgFile;

            var fileDialogResult = fileDialog.ShowDialog();

            if (fileDialogResult == true)
            {
                rpkgFile = fileDialog.FileName.Substring(fileDialog.FileName.LastIndexOf("\\") + 1);
                string baseFileName = rpkgFile.Substring(0, rpkgFile.LastIndexOf("."));
                string folderPath = fileDialog.FileName.Substring(0, fileDialog.FileName.LastIndexOf("\\") + 1);
                string parentFolderPath = folderPath;
                folderPath += baseFileName + "\\";

                System.IO.Directory.CreateDirectory(folderPath);

                int return_value = generate_temp_file_from_data(folderPath + tempFileName);

                string command = "-generate_rpkg_from";
                string input_path = folderPath;
                string filter = "";
                string search = "";
                string search_type = "";
                string output_path = parentFolderPath;

                execute_task rpkgExecute = task_execute;

                IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

                Progress progress = new Progress();

                progress.message.Content = "Generating RPKG file " + rpkgFile + "...";

                progress.ShowDialog();

                if (return_value == 0)
                {
                    MessageBoxShow("RPKG file successfully generated: " + fileDialog.FileName);
                }
                else
                {
                    MessageBoxShow("Error generating RPKG file: " + fileDialog.FileName);
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

        public List<string> controlNames;
        public int controlCount = 0;
        public string tempFileName = "";
        public string tbluFileName = "";
        public string rpkgFilePath = "";
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
            TEMP_TBLU_TOO_MANY
        };

        public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "task_execute", CallingConvention = CallingConvention.Cdecl)]
        public static extern int task_execute(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "reset_task_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_status();

        [DllImport("rpkg.dll", EntryPoint = "clear_temp_tblu_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_temp_tblu_data();

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent(UInt32 logical_parent);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_data(UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_reference_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_reference_data(UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "update_temp_file", CallingConvention = CallingConvention.Cdecl)]
        public static extern int update_temp_file(string offset, string type, string value);

        [DllImport("rpkg.dll", EntryPoint = "generate_temp_file_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_temp_file_from_data(string temp_file_path);

        [DllImport("rpkg.dll", EntryPoint = "is_offset_shared", CallingConvention = CallingConvention.Cdecl)]
        public static extern int is_offset_shared(string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_shared_values", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_shared_values(string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "get_shared_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_shared_index(string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "get_shared_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_shared_count(string offset, string property_type_index);

        [DllImport("rpkg.dll", EntryPoint = "update_temp_file_pointer", CallingConvention = CallingConvention.Cdecl)]
        public static extern int update_temp_file_pointer(string entry_index, string property_index, string offset);
    }
}

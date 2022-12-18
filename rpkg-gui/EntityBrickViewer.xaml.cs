using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using MahApps.Metro.Controls;
using System.Globalization;
using System.Windows.Forms;
using System.Windows.Threading;
using Brushes = System.Windows.Media.Brushes;
using Button = System.Windows.Controls.Button;
using Color = System.Windows.Media.Color;
using ComboBox = System.Windows.Controls.ComboBox;
using TextBox = System.Windows.Controls.TextBox;
using TextBlock = System.Windows.Controls.TextBlock;

// ReSharper disable SuggestVarOrType_Elsewhere
// ReSharper disable LocalizableElement
// ReSharper disable UseObjectOrCollectionInitializer

namespace rpkg
{
    /// <summary>
    /// Interaction logic for EntityBrickViewer.xaml
    /// </summary>
    public partial class EntityBrickViewer : MetroWindow
    {
        public EntityBrickViewer()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            switch (currentThemeBrightness)
            {
                case "Dark":
                    MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                    MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
                    break;
                case "Light":
                    MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
                    MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                    break;
            }

            LoadLoadingWindow();
        }

        private void LoadLoadingWindow()
        {
            if (temp_file_version == 0)
            {
                MessageBoxShow("Error: Could not determine the TEMP/TBLU file version.");
            }

            message = new Message();

            message.message.Content = "Please wait for all entity nodes to load.\n\nThis window will disappear once all the nodes have been loaded.";

            message.Topmost = true;

            message.Show();

            if (loadingWindowTimer == null)
            {
                loadingWindowTimer = new DispatcherTimer();

                loadingWindowTimer.Interval = TimeSpan.FromMilliseconds(400);

                loadingWindowTimer.Tick += LoadingWindowTimer_Tick;
            }

            loadingWindowTimer.Stop();
            loadingWindowTimer.Start();
        }

        private void LoadingWindowTimer_Tick(object sender, EventArgs e)
        {
            if (!(sender is DispatcherTimer timer))
            {
                return;
            }

            timer.Stop();

            LoadMainTreeView();
        }

        private void MetroWindow_Activated(object sender, EventArgs e)
        {
            if (!hidden) return;

            switch (currentThemeBrightness)
            {
                case "Dark":
                    MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                    MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
                    break;
                case "Light":
                    MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
                    MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                    break;
            }

            LoadLoadingWindow();

            hidden = false;
        }

        private void LoadImageList()
        {
            ImageList imageList = new ImageList();
            imageList.ImageSize = new System.Drawing.Size(16, 16);
            Console.WriteLine();
            imageList.Images.Add("empty", Properties.Resources.dots);
            imageList.Images.Add("entity", Properties.Resources.bullet_black);
            imageList.Images.Add("actor", Properties.Resources.bricks);
            imageList.Images.Add("speak", Properties.Resources.sound);
            imageList.Images.Add("aiarea", Properties.Resources.shape_square);
            imageList.Images.Add("disguise", Properties.Resources.timeline_marker);

            MainTreeView.ImageList = imageList;
        }

        private void LoadMainTreeView()
        {
            MainTreeView.Nodes.Clear();

            while (MainStackPanelTEMP.Children.Count > 0)
            {
                MainStackPanelTEMP.Children.RemoveAt(MainStackPanelTEMP.Children.Count - 1);
            }

            visitedNodes = null;
            visitedNodes = new List<TreeNode>();

            controls = null;
            controls = new List<string>();
            controlsChanged = null;
            controlsChanged = new List<bool>();
            controlsTEMPIndexes = null;
            controlsTEMPIndexes = new List<uint>();
            controlJSONPointers = null;
            controlJSONPointers = new List<string>();
            controlJSONPointersTypes = null;
            controlJSONPointersTypes = new List<string>();
            controlZGuids = null;
            controlZGuids = new List<ZGuid>();

            controlCount = 0;

            MainTreeView.BeginUpdate();

            MainTreeView.AfterSelect += MainTreeView_AfterSelect;

            {
                var topItem = new TreeNode();

                topItem.Text = tempFileNameFull.Replace("(", "").Replace(")", "");

                string responseString = Marshal.PtrToStringAnsi(get_top_level_logical_parents(temps_index));

                string[] topLevelParents = responseString.Trim(',').Split(',');

                foreach (string topLevelParent in topLevelParents)
                {
                    uint logical_parent = 0xFFFFFFFF;

                    uint.TryParse(topLevelParent, out logical_parent);

                    int entry_data_size = get_entries_with_logical_parent(temps_index, logical_parent);

                    byte[] entry_data = new byte[entry_data_size];

                    Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                    int data_pointer = 0;

                    while (data_pointer < entry_data_size)
                    {
                        uint entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        uint entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                        data_pointer += (int)entryNameLength;

                        if (MainTreeView.ImageList == null) LoadImageList();

                        var item = new TreeNode();

                        item.Text = entryName + " (" + entryIndex + ")";
                        item.Name = entryName;
                        item.Tag = entryIndex + "," + temps_index;
                        
                        LoadTreeView(ref item);

                        topItem.Nodes.Add(item);
                    }
                }

                int nodeCount = topItem.Nodes.Count;
                TreeNode[] nodes = new TreeNode[nodeCount];
                topItem.Nodes.CopyTo(nodes, 0);

                MainTreeView.Nodes.Add(topItem);
            }

            {
                string responseString = Marshal.PtrToStringAnsi(get_all_bricks(temps_index));

                if (responseString != "")
                {
                    string[] bricks = responseString.Trim(',').Split(',');

                    foreach (string brick in bricks)
                    {
                        string[] brickData = brick.Split(' ');

                        int temp_index_hash_reference = get_temp_index(brickData[0]);

                        var topItem = new TreeNode();

                        topItem.Text = brick;

                        responseString = Marshal.PtrToStringAnsi(get_top_level_logical_parents((uint)temp_index_hash_reference));

                        string[] topLevelParents = responseString.Trim(',').Split(',');

                        foreach (string topLevelParent in topLevelParents)
                        {
                            uint logical_parent = 0xFFFFFFFF;

                            uint.TryParse(topLevelParent, out logical_parent);

                            int entry_data_size = get_entries_with_logical_parent((uint)temp_index_hash_reference, logical_parent);

                            byte[] entry_data = new byte[entry_data_size];

                            Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                            int data_pointer = 0;

                            while (data_pointer < entry_data_size)
                            {
                                uint entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                                data_pointer += 4;

                                uint entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                                data_pointer += 4;

                                string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                                data_pointer += (int)entryNameLength;

                                if (MainTreeView.ImageList == null) LoadImageList();

                                var item = new TreeNode();

                                item.Text = entryName + " (" + entryIndex + ")";
                                item.Name = entryName;
                                item.Tag = entryIndex + "," + temp_index_hash_reference;

                                LoadTreeView(ref item);

                                topItem.Nodes.Add(item);
                            }
                        }

                        int nodeCount = topItem.Nodes.Count;
                        TreeNode[] nodes = new TreeNode[nodeCount];
                        topItem.Nodes.CopyTo(nodes, 0);

                        MainTreeView.Nodes.Add(topItem);
                    }
                }
            }

            MainTreeView.EndUpdate();

            treeViewBackup = new TreeViewBackup(MainTreeView.Nodes);

            message.Close();

            int temp_subentity_count = get_temp_subentity_count(temps_index);

            int tblu_subentity_count = get_tblu_subentity_count(temps_index);

            if (temp_subentity_count != tblu_subentity_count)
            {
                MessageBoxShow("Warning: There is a subEntities mismatch between the TEMP and TBLU files.\n\nThe TEMP file has " + temp_subentity_count + " subEntities\n\nThe TBLU file has " + tblu_subentity_count + " subEntities\n\nHaving mismatched subEntities (nodes in the treeview) counts can result in the nodes being misnamed.\n\nAlways look for the newest version of a TEMP file in the patch archives for a given chunk/dlc RPKG.");
            }
        }

        private void LoadTreeView(ref TreeNode masterTreeViewItem)
        {
            List<TreeNode> nodesToAdd = new List<TreeNode>();

            string nodeData = masterTreeViewItem.Tag.ToString();

            uint.TryParse(nodeData.Split(',')[0], out var temp_entryIndex);

            uint.TryParse(nodeData.Split(',')[1], out var temp_temp_index);

            string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_references(temp_temp_index, temp_entryIndex));

            Debug.Assert(hashReferenceData != null, nameof(hashReferenceData) + " != null");
            string[] hashReferences = hashReferenceData.Split(',');

            int temp_index_hash_reference = get_temp_index(hashReferences[0]);

            if (temp_index_hash_reference > 0)
            {
                uint logical_parent = 0xFFFFFFFF;

                int hash_entry_data_size = get_entries_with_logical_parent((uint)temp_index_hash_reference, logical_parent);

                if (hash_entry_data_size > 0)
                {
                    byte[] entry_data = new byte[hash_entry_data_size];

                    Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, hash_entry_data_size);

                    int data_pointer = 0;

                    while (data_pointer < hash_entry_data_size)
                    {
                        uint entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        uint entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                        data_pointer += (int)entryNameLength;

                        var itemHashReference = new TreeNode();

                        itemHashReference.Text = entryName + " (" + entryIndex + ") (" + temp_index_hash_reference + ")";
                        itemHashReference.Name = entryName;
                        itemHashReference.Tag = entryIndex + "," + temp_index_hash_reference;

                        LoadTreeView(ref itemHashReference);

                        nodesToAdd.Add(itemHashReference);
                    }
                }
            }

            int entry_data_size = get_entries_with_logical_parent(temp_temp_index, temp_entryIndex);

            if (entry_data_size > 0)
            {
                byte[] entry_data = new byte[entry_data_size];

                Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                int data_pointer = 0;

                while (data_pointer < entry_data_size)
                {
                    uint entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                    data_pointer += 4;

                    uint entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                    data_pointer += 4;

                    string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                    data_pointer += (int)entryNameLength;

                    var item2 = new TreeNode();

                    item2.Text = entryName + " (" + entryIndex + ")";
                    item2.Name = entryName;
                    item2.Tag = entryIndex + "," + temp_temp_index;

                    hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_references(temp_temp_index, entryIndex));
                    Debug.Assert(hashReferenceData != null, nameof(hashReferenceData) + " != null");
                    hashReferences = hashReferenceData.Split(',');

                    LoadTreeView(ref item2);

                    loadNodeIcon(ref item2, hashReferences[0]);

                    nodesToAdd.Add(item2);
                }
            }

            nodesToAdd = nodesToAdd.OrderBy(tn => tn.Name).ToList();

            foreach (TreeNode tn in nodesToAdd)
            {
                masterTreeViewItem.Nodes.Add(tn);
            }
        }

        private void loadNodeIcon(ref TreeNode tn, string hashReference)
        {
            int iconIndex = 0;
            switch (hashReference)
            {
                case "0031B66C16E8815D.CPPT": //zentity class
                    iconIndex = 1;
                    break;

                case "00DDCC74AD917CAA.TEMP": //actor.template
                    iconIndex = 2;
                    break;

                case "001782DC89EA0A62.CPPT": //sounddefs class
                    iconIndex = 3;
                    break;

                case "000F13E2D42C882E.CPPT": //aiareaentity class
                    iconIndex = 4;
                    break;

                case "002C17D9CCE5419D.CPPT": //disguisezoneentity class
                    iconIndex = 5;
                    break;
                //more could be added here
            }

            tn.ImageIndex = iconIndex;
            tn.SelectedImageIndex = iconIndex;
        }

        private void GoToNode_Click(object sender, RoutedEventArgs e)
        {
            bool foundNode = false;

            string nodeName = (sender as Button)?.Tag.ToString();

            Message tempMessage = new Message();

            tempMessage.message.Content = "Locating " + nodeName + "...";

            tempMessage.Show();

            foreach (TreeNode node in MainTreeView.Nodes)
            {
                if (!foundNode)
                {
                    GoToNode(node, ref nodeName, ref foundNode);
                }
            }

            tempMessage.Close();
        }

        void GoToNode(TreeNode mainNode, ref string nodeName, ref bool foundNode)
        {
            foreach (TreeNode node in mainNode.Nodes)
            {
                if (foundNode) continue;

                GoToNode(node, ref nodeName, ref foundNode);

                if (!node.Text.StartsWith(nodeName)) continue;

                foundNode = true;

                MainTreeView.Focus();

                MainTreeView.SelectedNode = node;
            }
        }

        private void PreviousNode_Click(object sender, RoutedEventArgs e)
        {
            if (visitedNodes.Count <= 1) return;

            TreeNode node;

            if (MainTreeView.SelectedNode == visitedNodes[visitedNodes.Count - 1] && visitedNodes.Count >= 2)
            {
                node = visitedNodes[visitedNodes.Count - 2];

                visitedNodes.RemoveAt(visitedNodes.Count - 1);
                visitedNodes.RemoveAt(visitedNodes.Count - 1);
            }
            else
            {
                node = visitedNodes[visitedNodes.Count - 1];

                visitedNodes.RemoveAt(visitedNodes.Count - 1);
            }

            MainTreeView.Focus();

            MainTreeView.SelectedNode = node;
        }

        private void MainTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (hidden) return;

            TreeNode item = e.Node;

            if (visitedNodes.Count == 0)
            {
                visitedNodes.Add(item);
            }
            else
            {
                if (visitedNodes[visitedNodes.Count - 1] != item)
                {
                    visitedNodes.Add(item);
                }
            }

            //item.IsSelected = true;

            if (MainTreeView.Nodes.Count <= 0) return;

            while (MainStackPanelTEMP.Children.Count > 0)
            {
                MainStackPanelTEMP.Children.RemoveAt(MainStackPanelTEMP.Children.Count - 1);
            }

            controls = null;
            controls = new List<string>();
            controlsChanged = null;
            controlsChanged = new List<bool>();
            controlsTEMPIndexes = null;
            controlsTEMPIndexes = new List<uint>();
            controlJSONPointers = null;
            controlJSONPointers = new List<string>();
            controlJSONPointersTypes = null;
            controlJSONPointersTypes = new List<string>();
            controlZGuids = null;
            controlZGuids = new List<ZGuid>();

            controlCount = 0;

            if (item.Text.Contains(".TEMP "))
            {
                string[] headerData = item.Text.Replace("(", "").Replace(")", "").Split(' ');

                string hashFileName = headerData[0];

                string ioiString = "";

                for (int i = 1; i < headerData.Length; i++)
                {
                    ioiString += headerData[i];

                    if (i != (headerData.Length - 1))
                    {
                        ioiString += " ";
                    }
                }

                temp_index = (uint)get_temp_index(hashFileName);

                TextBlock label1 = new TextBlock();
                label1.Text = hashFileName + "'s Data:";
                label1.FontSize = 18;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                label1 = new TextBlock();
                label1.Text = "IOI String:";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                TextBlock label2 = new TextBlock();
                label2.Text = ioiString;

                MainStackPanelTEMP.Children.Add(label2);

                label1 = new TextBlock();
                label1.Text = "Property data (subType):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput_TEMP("subType", "int32");

                label1 = new TextBlock();
                label1.Text = "Property data (blueprintIndexInResourceHeader):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput_TEMP("blueprintIndexInResourceHeader", "int32");

                label1 = new TextBlock();
                label1.Text = "Property data (rootEntityIndex):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput_TEMP("rootEntityIndex", "int32");

                label1 = new TextBlock();
                label1.Text = "Property data (propertyOverrides):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput_TEMP("propertyOverrides", "TArray<SEntityTemplatePropertyOverride>");

                label1 = new TextBlock();
                label1.Text = "Property data (externalSceneTypeIndicesInResourceHeader):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput_TEMP("externalSceneTypeIndicesInResourceHeader", "TArray<int32>");
            }
            else if (item.Text.Contains("("))
            {
                string entityName = item.Name;

                string nodeData = item.Tag.ToString();

                uint.TryParse(nodeData.Split(',')[0], out var entryIndex);

                entity_index = entryIndex;

                temp_index = 0;

                uint.TryParse(nodeData.Split(',')[1], out temp_index);

                string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_reference_data(temp_index, entryIndex));

                TextBlock label1 = new TextBlock();
                label1.Text = entityName + "'s Data:";
                label1.FontSize = 18;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                label1 = new TextBlock();
                label1.Text = "Hash reference data:";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                TextBlock label2 = new TextBlock();
                label2.Text = hashReferenceData;

                MainStackPanelTEMP.Children.Add(label2);

                label1 = new TextBlock();
                label1.Text = "Property data (propertyValues):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput(entryIndex, "propertyValues");

                label1 = new TextBlock();
                label1.Text = "Property data (postInitPropertyValues):";
                label1.FontSize = 16;
                label1.FontWeight = FontWeights.Bold;

                MainStackPanelTEMP.Children.Add(label1);

                AppendInput(entryIndex, "postInitPropertyValues");

                if (temp_file_version == 3)
                {
                    label1 = new TextBlock();
                    label1.Text = "Property data (platformSpecificPropertyValues):";
                    label1.FontSize = 16;
                    label1.FontWeight = FontWeights.Bold;

                    MainStackPanelTEMP.Children.Add(label1);

                    AppendInput(entryIndex, "platformSpecificPropertyValues");
                }
            }
        }

        private void AppendInput_TEMP(string valueType, string typeString)
        {
            int entry_data_size = get_temp_entries(temp_index, valueType, typeString);

            string responseString = Marshal.PtrToStringAnsi(get_response_string());

            if (responseString != "")
            {
                MessageBoxShow(responseString);
            }
            else
            {
                if (entry_data_size > 0)
                {
                    byte[] entry_data = new byte[entry_data_size];

                    Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                    int data_pointer = 0;

                    List<string> propertyValuesNumbers = new List<string>();
                    List<string> propertyValuePropertyIDs = new List<string>();
                    List<string> propertyValueTypes = new List<string>();
                    List<string> propertyValuePropertyIDsInput = new List<string>();
                    List<string> propertyValueTypesInput = new List<string>();
                    List<string> propertyValuesInput = new List<string>();

                    while (data_pointer < entry_data_size)
                    {
                        uint entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        bool added = false;

                        if (!propertyValuesNumbers.Contains(entryData))
                        {
                            propertyValuesNumbers.Add(entryData);

                            added = true;
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        propertyValuePropertyIDsInput.Add(entryData);

                        if (added)
                        {
                            propertyValuePropertyIDs.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        propertyValueTypesInput.Add(entryData);

                        if (added)
                        {
                            propertyValueTypes.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        propertyValuesInput.Add(entryData);
                    }

                    List<string>[] propertyValues = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueVals = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueValNames = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueJSONPointers = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueJSONPointersTypes = new List<string>[propertyValuesNumbers.Count];

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        propertyValues[i] = new List<string>();
                        propertyValueVals[i] = new List<string>();
                        propertyValueValNames[i] = new List<string>();
                        propertyValueJSONPointers[i] = new List<string>();
                        propertyValueJSONPointersTypes[i] = new List<string>();
                    }

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        for (int j = 0; j < propertyValuesInput.Count; j++)
                        {
                            string[] propertyValue = propertyValuesInput[j].Split(' ');

                            if (propertyValue.Length >= 3)
                            {
                                string propertyValuesString = valueType;

                                int position2 = propertyValue[0].IndexOf(propertyValuesString);

                                if (position2 < 0) continue;

                                propertyValues[i].Add(propertyValuesInput[j]);

                                propertyValueValNames[i].Add(propertyValue[0].Substring(position2 + propertyValuesString.Length));

                                propertyValueJSONPointers[i].Add(propertyValue[0]);

                                propertyValueJSONPointersTypes[i].Add(propertyValue[1]);

                                string valueString = "";

                                for (int k = 2; k < propertyValue.Length; k++)
                                {
                                    valueString += propertyValue[k];

                                    if (k != (propertyValue.Length - 1))
                                    {
                                        valueString += " ";
                                    }
                                }

                                propertyValueVals[i].Add(valueString);
                            }
                            else
                            {
                                MessageBoxShow("Error: Property value string is malformed: " + propertyValues[j]);
                            }
                        }
                    }

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        if (propertyValueVals[i].Count <= 0) continue;

                        string enumValues = Marshal.PtrToStringAnsi(get_enum_values(temp_index, propertyValueTypes[i]));

                        if (enumValues != "")
                        {
                            AppendInput_enum(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, ref enumValues);
                        }
                        else switch (propertyValueTypes[i])
                        {
                            case "bool":
                                AppendInput_bool(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            case "ZGuid":
                                AppendInput_ZGuid(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            default:
                                AppendInput_Default(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                        }
                    }
                }
                else
                {
                    TextBlock label2 = new TextBlock();
                    label2.Text = "  None";

                    MainStackPanelTEMP.Children.Add(label2);
                }
            }
        }

        private void AppendInput(uint entryIndex, string valueType)
        {
            int entry_data_size = get_entries(temp_index, entryIndex, valueType);

            string responseString = Marshal.PtrToStringAnsi(get_response_string());

            if (responseString != "")
            {
                MessageBoxShow(responseString);
            }
            else
            {
                if (entry_data_size > 0)
                {
                    byte[] entry_data = new byte[entry_data_size];

                    Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                    int data_pointer = 0;

                    List<string> propertyValuesNumbers = new List<string>();
                    List<string> propertyValuePropertyIDs = new List<string>();
                    List<string> propertyValueTypes = new List<string>();
                    List<string> propertyValuePropertyIDsInput = new List<string>();
                    List<string> propertyValueTypesInput = new List<string>();
                    List<string> propertyValuesInput = new List<string>();

                    while (data_pointer < entry_data_size)
                    {
                        uint entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        bool added = false;

                        if (!propertyValuesNumbers.Contains(entryData))
                        {
                            propertyValuesNumbers.Add(entryData);

                            added = true;
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        propertyValuePropertyIDsInput.Add(entryData);

                        if (added)
                        {
                            propertyValuePropertyIDs.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        propertyValueTypesInput.Add(entryData);

                        if (added)
                        {
                            propertyValueTypes.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        propertyValuesInput.Add(entryData);
                    }

                    List<string>[] propertyValues = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueVals = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueValNames = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueJSONPointers = new List<string>[propertyValuesNumbers.Count];
                    List<string>[] propertyValueJSONPointersTypes = new List<string>[propertyValuesNumbers.Count];

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        propertyValues[i] = new List<string>();
                        propertyValueVals[i] = new List<string>();
                        propertyValueValNames[i] = new List<string>();
                        propertyValueJSONPointers[i] = new List<string>();
                        propertyValueJSONPointersTypes[i] = new List<string>();
                    }

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        for (int j = 0; j < propertyValuesInput.Count; j++)
                        {
                            string[] propertyValue = propertyValuesInput[j].Split(' ');

                            if (propertyValue.Length >= 3)
                            {
                                string propertyValuesString = valueType + "/" + propertyValuesNumbers[i] + "/value/$val";

                                int position2 = propertyValue[0].IndexOf(propertyValuesString);

                                if (position2 < 0) continue;

                                propertyValues[i].Add(propertyValuesInput[j]);

                                propertyValueValNames[i].Add(propertyValue[0].Substring(position2 + propertyValuesString.Length));

                                propertyValueJSONPointers[i].Add(propertyValue[0]);

                                propertyValueJSONPointersTypes[i].Add(propertyValue[1]);

                                string valueString = "";

                                for (int k = 2; k < propertyValue.Length; k++)
                                {
                                    valueString += propertyValue[k];

                                    if (k != (propertyValue.Length - 1))
                                    {
                                        valueString += " ";
                                    }
                                }

                                propertyValueVals[i].Add(valueString);
                            }
                            else
                            {
                                MessageBoxShow("Error: Property value string is malformed: " + propertyValues[j]);
                            }
                        }
                    }

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        if (propertyValueVals[i].Count <= 0) continue;

                        string enumValues = Marshal.PtrToStringAnsi(get_enum_values(temp_index, propertyValueTypes[i]));

                        if (enumValues != "")
                        {
                            AppendInput_enum(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, ref enumValues);
                        }
                        else switch (propertyValueTypes[i])
                        {
                            case "bool":
                                AppendInput_bool(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            case "SColorRGB":
                                AppendInput_SColorRGB(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, false);
                                break;
                            case "SColorRGBA":
                                AppendInput_SColorRGB(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, true);
                                break;
                            case "SMatrix43":
                                AppendInput_SMatrix43(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            case "SVector2":
                                AppendInput_SVector2(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            case "SVector3":
                                AppendInput_SVector3(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            case "SVector4":
                                AppendInput_SVector4(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            case "ZGuid":
                                AppendInput_ZGuid(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                            default:
                                AppendInput_Default(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                                break;
                        }
                    }
                }
                else
                {
                    TextBlock label2 = new TextBlock();
                    label2.Text = "  None";

                    MainStackPanelTEMP.Children.Add(label2);
                }
            }
        }

        #region build and append entries

        private void AppendInput_Default(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            Grid grid = new Grid();

            ColumnDefinition columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);

            int rowCount = 1;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                //filters these out of the gui, since they are not needed
                if (propertyValueValNames[propertyIndex][i].Contains("/entityID")) continue;
                if (propertyValueValNames[propertyIndex][i].Contains("/externalSceneIndex")) continue;
                if (propertyValueValNames[propertyIndex][i].Contains("/exposedEntity")) continue;

                if (propertyValueValNames[propertyIndex][i].Length > 0)
                {
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
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);

                    TextBlock label = new TextBlock();
                    label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":";
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    string enumValues = Marshal.PtrToStringAnsi(get_enum_values(temp_index, propertyValueTypes[propertyIndex]));

                    if (enumValues != "")
                    {
                        ComboBox comboBox = new ComboBox();
                        comboBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);

                        string[] enumValuesStrings = enumValues.Trim(',').Split(',');

                        int index = 0;
                        int indexCount = 0;

                        foreach (string enumValue in enumValuesStrings)
                        {
                            if (propertyValueVals[propertyIndex][i] == enumValue)
                            {
                                index = indexCount;
                            }

                            indexCount++;

                            comboBox.Items.Add(enumValue);
                        }

                        comboBox.SelectedIndex = index;

                        comboBox.Margin = new Thickness(4, 0, 4, 0);
                        comboBox.IsReadOnly = true;
                        comboBox.IsEditable = false;
                        grid.Children.Add(comboBox);
                        Grid.SetRow(comboBox, rowCount);
                        Grid.SetColumn(comboBox, 1);
                        Grid.SetColumnSpan(comboBox, 3);
                    }
                    else if (propertyValueJSONPointersTypes[propertyIndex][i] == "bool")
                    {
                        ComboBox comboBox = new ComboBox();
                        comboBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                        comboBox.Items.Add("True");
                        comboBox.Items.Add("False");

                        if (propertyValueVals[propertyIndex][i] == "0")
                        {
                            comboBox.SelectedIndex = 1;
                        }
                        else
                        {
                            comboBox.SelectedIndex = 0;
                        }

                        comboBox.Margin = new Thickness(4, 0, 4, 0);
                        comboBox.IsReadOnly = true;
                        comboBox.IsEditable = false;
                        grid.Children.Add(comboBox);
                        Grid.SetRow(comboBox, rowCount);
                        Grid.SetColumn(comboBox, 1);
                        Grid.SetColumnSpan(comboBox, 3);
                    }
                    else
                    {
                        if (propertyValueValNames[propertyIndex][i].Contains("/entityIndex"))
                        {
                            int.TryParse(propertyValueVals[propertyIndex][i], out var temp_entity_index);

                            string temp_entity_name = Marshal.PtrToStringAnsi(get_entry_name(temp_index, temp_entity_index));

                            if (temp_entity_name == "")
                            {
                                TextBox textBox = new TextBox();
                                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                                textBox.Text = propertyValueVals[propertyIndex][i];
                                textBox.Margin = new Thickness(4, 0, 4, 0);
                                textBox.IsReadOnly = true;
                                grid.Children.Add(textBox);
                                Grid.SetRow(textBox, rowCount);
                                Grid.SetColumn(textBox, 1);
                                Grid.SetColumnSpan(textBox, 3);
                            }
                            else
                            {
                                TextBox textBox = new TextBox();
                                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                                textBox.Text = propertyValueVals[propertyIndex][i];
                                textBox.Margin = new Thickness(4, 0, 4, 0);
                                textBox.IsReadOnly = true;
                                grid.Children.Add(textBox);
                                Grid.SetRow(textBox, rowCount);
                                Grid.SetColumn(textBox, 1);

                                textBox = new TextBox();
                                Debug.Assert(temp_entity_name != null, nameof(temp_entity_name) + " != null");
                                textBox.Text = temp_entity_name;
                                textBox.Margin = new Thickness(4, 0, 4, 0);
                                textBox.IsReadOnly = true;
                                grid.Children.Add(textBox);
                                Grid.SetRow(textBox, rowCount);
                                Grid.SetColumn(textBox, 2);

                                Button button = new Button();
                                button.Content = "  GO  ";
                                button.Margin = new Thickness(4, 0, 4, 0);
                                button.Background = Brushes.Red;
                                button.Tag = temp_entity_name + " (" + propertyValueVals[propertyIndex][i] + ")";
                                button.Click += GoToNode_Click;
                                grid.Children.Add(button);
                                Grid.SetRow(button, rowCount);
                                Grid.SetColumn(button, 3);
                            }
                        }
                        else
                        {
                            TextBox textBox = new TextBox();
                            textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                            textBox.Text = propertyValueVals[propertyIndex][i];
                            textBox.Margin = new Thickness(4, 0, 4, 0);
                            textBox.IsReadOnly = true;
                            grid.Children.Add(textBox);
                            Grid.SetRow(textBox, rowCount);
                            Grid.SetColumn(textBox, 1);
                            Grid.SetColumnSpan(textBox, 3);
                        }
                    }

                    rowCount += 4;
                }
                else
                {
                    RowDefinition rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);
                    rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                    grid.RowDefinitions.Add(rowDefinition);
                    rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);


                    TextBlock label = new TextBlock();
                    label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":";
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    TextBox textBox = new TextBox();
                    textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                    textBox.Text = propertyValueVals[propertyIndex][i];
                    textBox.Margin = new Thickness(4, 0, 4, 0);
                    textBox.IsReadOnly = true;
                    grid.Children.Add(textBox);
                    Grid.SetRow(textBox, rowCount);
                    Grid.SetColumn(textBox, 1);
                    Grid.SetColumnSpan(textBox, 3);

                    rowCount += 3;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_bool(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            Grid grid = new Grid();

            ColumnDefinition columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);

            int rowCount = 1;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                if (propertyValueValNames[propertyIndex][i].Length > 0)
                {
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
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);

                    TextBlock label = new TextBlock();
                    label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    ComboBox comboBox = new ComboBox();
                    comboBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                    comboBox.Items.Add("True");
                    comboBox.Items.Add("False");

                    if (propertyValueVals[propertyIndex][i] == "0")
                    {
                        comboBox.SelectedIndex = 1;
                    }
                    else
                    {
                        comboBox.SelectedIndex = 0;
                    }

                    comboBox.Margin = new Thickness(4, 0, 4, 0);
                    comboBox.IsReadOnly = true;
                    comboBox.IsEditable = false;
                    grid.Children.Add(comboBox);
                    Grid.SetRow(comboBox, rowCount);
                    Grid.SetColumn(comboBox, 1);
                    Grid.SetColumnSpan(comboBox, 2);

                    rowCount += 4;
                }
                else
                {
                    RowDefinition rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);
                    rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                    grid.RowDefinitions.Add(rowDefinition);
                    rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);

                    TextBlock label = new TextBlock();
                    label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    ComboBox comboBox = new ComboBox();
                    comboBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                    comboBox.Items.Add("True");
                    comboBox.Items.Add("False");

                    if (propertyValueVals[propertyIndex][i] == "0")
                    {
                        comboBox.SelectedIndex = 1;
                    }
                    else
                    {
                        comboBox.SelectedIndex = 0;
                    }

                    comboBox.Margin = new Thickness(4, 0, 4, 0);
                    comboBox.IsReadOnly = true;
                    comboBox.IsEditable = false;
                    grid.Children.Add(comboBox);
                    Grid.SetRow(comboBox, rowCount);
                    Grid.SetColumnSpan(comboBox, 2);
                    Grid.SetColumn(comboBox, 1);

                    rowCount += 3;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_enum(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes, ref string enumValues)
        {
            Grid grid = new Grid();

            ColumnDefinition columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);

            int rowCount = 1;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                if (propertyValueValNames[propertyIndex][i].Length > 0)
                {
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
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);

                    TextBlock label = new TextBlock();
                    label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    ComboBox comboBox = new ComboBox();
                    comboBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);

                    string[] enumValuesStrings = enumValues.Trim(',').Split(',');

                    int index = 0;
                    int indexCount = 0;

                    foreach (string enumValue in enumValuesStrings)
                    {
                        if (propertyValueVals[propertyIndex][i] == enumValue)
                        {
                            index = indexCount;
                        }

                        indexCount++;

                        comboBox.Items.Add(enumValue);
                    }

                    comboBox.SelectedIndex = index;

                    comboBox.Margin = new Thickness(4, 0, 4, 0);
                    comboBox.IsReadOnly = true;
                    comboBox.IsEditable = false;
                    grid.Children.Add(comboBox);
                    Grid.SetRow(comboBox, rowCount);
                    Grid.SetColumn(comboBox, 1);
                    Grid.SetColumnSpan(comboBox, 2);

                    rowCount += 4;
                }
                else
                {
                    RowDefinition rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);
                    rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                    grid.RowDefinitions.Add(rowDefinition);
                    rowDefinition = new RowDefinition();
                    rowDefinition.Height = new GridLength(8);
                    grid.RowDefinitions.Add(rowDefinition);

                    TextBlock label = new TextBlock();
                    label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    ComboBox comboBox = new ComboBox();
                    comboBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);

                    string[] enumValuesStrings = enumValues.Trim(',').Split(',');

                    int index = 0;
                    int indexCount = 0;

                    foreach (string enumValue in enumValuesStrings)
                    {
                        if (propertyValueVals[propertyIndex][i] == enumValue)
                        {
                            index = indexCount;
                        }

                        indexCount++;

                        comboBox.Items.Add(enumValue);
                    }

                    comboBox.SelectedIndex = index;

                    comboBox.Margin = new Thickness(4, 0, 4, 0);
                    comboBox.IsReadOnly = true;
                    comboBox.IsEditable = false;
                    grid.Children.Add(comboBox);
                    Grid.SetRow(comboBox, rowCount);
                    Grid.SetColumn(comboBox, 1);
                    Grid.SetColumnSpan(comboBox, 2);

                    rowCount += 3;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SColorRGB(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes, bool rgba_mode)
        {
            TextBlock label1 = new TextBlock();

            label1.Text = propertyValuePropertyIDs[propertyIndex] + ":";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanelTEMP.Children.Add(label1);

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

            if (rgba_mode)
            {
                columnDefinition = new ColumnDefinition();
                columnDefinition.Width = GridLength.Auto;
                grid.ColumnDefinitions.Add(columnDefinition);
                columnDefinition = new ColumnDefinition();
                columnDefinition.Width = new GridLength(1, GridUnitType.Star);
                grid.ColumnDefinitions.Add(columnDefinition);
            }

            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            int columnCount = 0;

            string colorPickerName = "";

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                TextBlock label = new TextBlock();
                label.Text = propertyValueValNames[propertyIndex][i] + " (" + propertyValueJSONPointersTypes[propertyIndex][i] + "):";
                grid.Children.Add(label);
                Grid.SetRow(label, 1);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);

                colorPickerName += textBox.Name + "x";

                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.IsReadOnly = true;
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, 1);
                Grid.SetColumn(textBox, columnCount);

                columnCount++;
            }

            float r = 0;
            float g = 0;
            float b = 0;
            float a = 0;

            float.TryParse(propertyValueVals[propertyIndex][0], NumberStyles.Float, CultureInfo.InvariantCulture, out r);
            float.TryParse(propertyValueVals[propertyIndex][1], NumberStyles.Float, CultureInfo.InvariantCulture, out g);
            float.TryParse(propertyValueVals[propertyIndex][2], NumberStyles.Float, CultureInfo.InvariantCulture, out b);

            if (rgba_mode)
            {
                float.TryParse(propertyValueVals[propertyIndex][3], NumberStyles.Float, CultureInfo.InvariantCulture, out a);
            }

            ColorCanvas colorCanvas = new ColorCanvas();
            colorCanvas.Name = "cc_" + colorPickerName;

            colorCanvas.R = FloatToByte(r);
            colorCanvas.G = FloatToByte(g);
            colorCanvas.B = FloatToByte(b);

            if (rgba_mode)
            {
                colorCanvas.A = FloatToByte(a);
            }

            colorCanvas.SelectedColorChanged += ColorCanvas_SelectedColorChanged;

            grid.Children.Add(colorCanvas);
            Grid.SetRow(colorCanvas, 3);

            Grid.SetColumnSpan(colorCanvas, rgba_mode ? 9 : 7);

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SMatrix43(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            TextBlock label1 = new TextBlock();

            label1.Text = propertyValuePropertyIDs[propertyIndex] + ":";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanelTEMP.Children.Add(label1);

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

            int rowCount = 1;

            int columnCount = 0;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                TextBlock label = new TextBlock();
                label.Text = propertyValueValNames[propertyIndex][i] + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.IsReadOnly = true;
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);

                columnCount++;

                if ((i + 1) % 3 == 0 && i > 0)
                {
                    rowCount += 2;
                }

                if (columnCount == 6)
                {
                    columnCount = 0;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SVector2(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            TextBlock label1 = new TextBlock();

            label1.Text = propertyValuePropertyIDs[propertyIndex] + ":";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanelTEMP.Children.Add(label1);

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

            int rowCount = 1;

            int columnCount = 0;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                TextBlock label = new TextBlock();
                label.Text = propertyValueValNames[propertyIndex][i] + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.IsReadOnly = true;
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);

                columnCount++;

                if ((i + 1) % 3 == 0 && i > 0)
                {
                    rowCount += 2;
                }

                if (columnCount == 6)
                {
                    columnCount = 0;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SVector3(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            TextBlock label1 = new TextBlock();

            label1.Text = propertyValuePropertyIDs[propertyIndex] + ":";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanelTEMP.Children.Add(label1);

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

            int rowCount = 1;

            int columnCount = 0;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                TextBlock label = new TextBlock();
                label.Text = propertyValueValNames[propertyIndex][i] + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.IsReadOnly = true;
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);

                columnCount++;

                if ((i + 1) % 3 == 0 && i > 0)
                {
                    rowCount += 2;
                }

                if (columnCount == 6)
                {
                    columnCount = 0;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SVector4(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            TextBlock label1 = new TextBlock();

            label1.Text = propertyValuePropertyIDs[propertyIndex] + ":";

            label1.FontSize = 14;
            label1.FontWeight = FontWeights.Bold;

            MainStackPanelTEMP.Children.Add(label1);

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
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);

            int rowCount = 1;

            int columnCount = 0;

            for (int i = 0; i < propertyValueVals[propertyIndex].Count; i++)
            {
                TextBlock label = new TextBlock();
                label.Text = propertyValueValNames[propertyIndex][i] + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.IsReadOnly = true;
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, columnCount);

                columnCount++;

                if ((i + 1) % 4 == 0 && i > 0)
                {
                    rowCount += 2;
                }

                if (columnCount == 8)
                {
                    columnCount = 0;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_ZGuid(uint temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            //get data type: propertyValueTypes[propertyIndex]

            Grid grid = new Grid();

            ColumnDefinition columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = new GridLength(1, GridUnitType.Star);
            grid.ColumnDefinitions.Add(columnDefinition);
            columnDefinition = new ColumnDefinition();
            columnDefinition.Width = GridLength.Auto;
            grid.ColumnDefinitions.Add(columnDefinition);

            int rowCount = 1;

            if (propertyValueVals[propertyIndex].Count == 11 && propertyValueValNames[propertyIndex].Count == 11)
            {
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
                rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(1, GridUnitType.Star);
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
                rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(1, GridUnitType.Star);
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
                rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(1, GridUnitType.Star);
                grid.RowDefinitions.Add(rowDefinition);
                rowDefinition = new RowDefinition();
                rowDefinition.Height = new GridLength(8);
                grid.RowDefinitions.Add(rowDefinition);

                TextBlock label = new TextBlock();
                label.Text = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, 0);

                TextBox textBox = new TextBox();
                textBox.Name = "ZGuid" + controlZGuids.Count;
                textBox.IsReadOnly = true;

                string zguidString = GenerateZGuid(ref propertyValueVals[propertyIndex]);

                ZGuid zguid = new ZGuid();
                zguid.controlName = textBox.Name;

                zguid.subControlNames = new string[11];

                zguid.zguidString = zguidString;

                textBox.Text = zguidString;
                textBox.Margin = new Thickness(4, 0, 4, 0);
                grid.Children.Add(textBox);
                Grid.SetRow(textBox, rowCount);
                Grid.SetColumn(textBox, 1);
                Grid.SetColumnSpan(textBox, 2);

                for (int t = 0; t < 11; t++)
                {
                    rowCount += 1;

                    textBox = new TextBox();
                    textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][t], propertyValueJSONPointersTypes[propertyIndex][t]);

                    zguid.subControlNames[t] = textBox.Name;

                    textBox.Text = propertyValueVals[propertyIndex][t];
                    textBox.Margin = new Thickness(4, 0, 4, 0);
                    //textBox.TextChanged += TextBox_TextChanged;
                    textBox.Visibility = Visibility.Collapsed;
                    grid.Children.Add(textBox);
                    Grid.SetRow(textBox, rowCount);
                    Grid.SetColumn(textBox, 1);
                }

                controlZGuids.Add(zguid);

                rowCount += 4;
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        #endregion

        private string GenerateZGuid(ref List<string> zguidData)
        {
            string zguidString = "";

            ZGuid zguid = new ZGuid();

            bool return_value = uint.TryParse(zguidData[0], out zguid._a);
            zguidString += zguid._a.ToString("X2").PadLeft(8, '0') + "-";

            return_value = ushort.TryParse(zguidData[1], out zguid._b);
            zguidString += zguid._b.ToString("X2").PadLeft(4, '0') + "-";

            return_value = ushort.TryParse(zguidData[2], out zguid._c);
            zguidString += zguid._c.ToString("X2").PadLeft(4, '0') + "-";

            return_value = byte.TryParse(zguidData[3], out zguid._d);
            zguidString += zguid._d.ToString("X2").PadLeft(2, '0');

            return_value = byte.TryParse(zguidData[4], out zguid._e);
            zguidString += zguid._e.ToString("X2").PadLeft(2, '0') + "-";

            return_value = byte.TryParse(zguidData[5], out zguid._f);
            zguidString += zguid._f.ToString("X2").PadLeft(2, '0');

            return_value = byte.TryParse(zguidData[6], out zguid._g);
            zguidString += zguid._g.ToString("X2").PadLeft(2, '0');

            return_value = byte.TryParse(zguidData[7], out zguid._h);
            zguidString += zguid._h.ToString("X2").PadLeft(2, '0');

            return_value = byte.TryParse(zguidData[8], out zguid._i);
            zguidString += zguid._i.ToString("X2").PadLeft(2, '0');

            return_value = byte.TryParse(zguidData[9], out zguid._j);
            zguidString += zguid._j.ToString("X2").PadLeft(2, '0');

            return_value = byte.TryParse(zguidData[10], out zguid._k);
            zguidString += zguid._k.ToString("X2").PadLeft(2, '0');

            return zguidString;
        }

        private string GetNewControlName(uint temp_index, string input, string type)
        {
            controlsTEMPIndexes.Add(temp_index);

            controlJSONPointers.Add(input);

            controlJSONPointersTypes.Add(type);

            controlsChanged.Add(false);

            string control = "c_" + controlCount;

            controls.Add(control);

            controlCount++;

            return control;
        }

        private static DependencyObject FindDescendant(DependencyObject parent, string name)
        {
            if ((parent is FrameworkElement element) && (element.Name == name))
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

        private void SearchTEMPsTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (searchTEMPsInputTimer == null)
            {
                searchTEMPsInputTimer = new DispatcherTimer();

                searchTEMPsInputTimer.Interval = TimeSpan.FromMilliseconds(600);

                searchTEMPsInputTimer.Tick += SearchTEMPsTextBox_TimerTimeout;
            }

            searchTEMPsInputTimer.Stop();
            searchTEMPsInputTimer.Tag = (sender as TextBox)?.Text;
            searchTEMPsInputTimer.Start();
        }

        private void SearchTEMPsTextBox_TimerTimeout(object sender, EventArgs e)
        {
            var timer = (sender as DispatcherTimer);

            if (timer == null)
            {
                return;
            }

            timer.Stop();

            MainTreeView.BeginUpdate();

            treeViewBackup.Restore();

            if (FilterTextBox.Text.Length > 0)
            {
                string filter = FilterTextBox.Text.ToLower();
                int childrenVisibleCount = 0;

                foreach (TreeNode node in MainTreeView.Nodes)
                {
                    FilterNodes(node, ref filter, childrenVisibleCount);
                }
            }

            MainTreeView.EndUpdate();
        }

        #region treeView navigation
        private static int FilterNodes(TreeNode parentNode, ref string filter, int childrenVisibleCount)
        {
            int currentChildrenVisibleCount = 0;

            if (parentNode == null) return childrenVisibleCount;
            
            if (parentNode.Nodes.Count > 0)
            {
                foreach (TreeNode node in parentNode.Nodes)
                {
                    currentChildrenVisibleCount += FilterNodes(node, ref filter, 0);
                }
            }

            if (!parentNode.Text.ToLower().Contains(filter) && currentChildrenVisibleCount == 0)
            {
                parentNode.Nodes.Clear();
            }
            else
            {
                if (parentNode.Nodes.Count > 0)
                {
                    List<int> nodesToRemove = new List<int>();

                    for (int i = 0; i < parentNode.Nodes.Count; i++)
                    {
                        if (!parentNode.Nodes[i].Text.ToLower().Contains(filter) && parentNode.Nodes[i].Nodes.Count < 1)
                        {
                            nodesToRemove.Add(i);
                        }
                    }

                    for (int i = 0; i < nodesToRemove.Count; i++)
                    {
                        int toRemove = nodesToRemove.Count - i - 1;

                        parentNode.Nodes[nodesToRemove[toRemove]].Remove();
                    }
                }

                childrenVisibleCount++;
            }

            return childrenVisibleCount;
        }

        private void ExpandAllNodes_Click(object sender, RoutedEventArgs e)
        {
            MainTreeView.ExpandAll();
        }

        private void CollapseAllNodes_Click(object sender, RoutedEventArgs e)
        {
            MainTreeView.CollapseAll();
        }

        #endregion

        private void MainTreeView_OnAfterSelect(object sender, TreeViewEventArgs e)
        {
            string nodePath = NodeToPath(e.Node);
            
            if (nodePath.Length <= 0) return;
            if (nodePath.EndsWith(@"\")) nodePath = nodePath.Substring(0, nodePath.Length - 1);
            
            NodePathTextBox.Text = nodePath;
        }

        public class TreeViewBackup : List<TreeViewBackup>
        {
            private TreeNode Parent { get; }
            private TreeNodeCollection Children { get; }

            public TreeViewBackup(TreeNodeCollection children, TreeNode parent = null)
            {
                Parent = parent;
                Children = children;
                AddRange(Children.Cast<TreeNode>().Select(child => new TreeViewBackup(child.Nodes, child)));
            }

            public void Restore()
            {
                Children.Clear();
                ForEach(clone => clone.Restore());
                Children.AddRange(this.Select(n => n.Parent).ToArray());
            }
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            clear_temp_tblu_data();
            hidden = true;
            Hide();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = true;

            clear_temp_tblu_data();
            hidden = true;
            Hide();
        }

        private static void MessageBoxShow(string messageBoxString)
        {
            Message messageBox = new Message();

            messageBox.message.Content = messageBoxString;

            messageBox.ShowDialog();
        }

        private void ColorCanvas_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            ColorCanvas colorCanvas = (sender as ColorCanvas);

            Debug.Assert(colorCanvas != null, nameof(colorCanvas) + " != null");
            string[] textBoxes = colorCanvas.Name.Replace("cc_", "").Split('x');

            DependencyObject descendant = FindDescendant(ViewerWindow, textBoxes[0]);
            ((TextBox) descendant).Text = StringByteToStringFloat(colorCanvas.R);

            descendant = FindDescendant(ViewerWindow, textBoxes[1]);
            ((TextBox) descendant).Text = StringByteToStringFloat(colorCanvas.G);

            descendant = FindDescendant(ViewerWindow, textBoxes[2]);
            ((TextBox) descendant).Text = StringByteToStringFloat(colorCanvas.B);

            if (textBoxes.Length == 5)
            {
                descendant = FindDescendant(ViewerWindow, textBoxes[3]);
                ((TextBox) descendant).Text = StringByteToStringFloat(colorCanvas.A);
            }
        }

        private void NodePathTextBox_OnTextChanged(object sender, TextChangedEventArgs e)
        {

        }

        #region util methods

        private static byte FloatToByte(float input)
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

        private static string StringByteToStringFloat(byte input)
        {
            int temp_int = input;

            float temp_float = temp_int / 255.0F;

            return temp_float.ToString(CultureInfo.InvariantCulture);
        }

        private static string NodeToPath(TreeNode node)
        {
            string nodePath = "";

            while (node.Parent != null)
            {
                nodePath = node.Name + @"\" + nodePath;
                node = node.Parent;
            }

            return nodePath;
        }

        private static string formatPropertyName(string propertyName)
        {
            return propertyName; // we have formatted the property name
        }

        #endregion

        public List<string> controls;
        public List<bool> controlsChanged;
        public List<uint> controlsTEMPIndexes;
        public List<string> controlJSONPointers;
        public List<string> controlJSONPointersTypes;
        public List<ZGuid> controlZGuids;
        public int controlCount = 0;
        public uint entity_index = 0;
        public uint temp_index = 0;
        public uint temps_index = 0;
        public int temp_file_version = 0;
        public string tempFileName = "";
        public string tbluFileName = "";
        public string rpkgFilePath = "";
        public string tempFileNameFull = "";
        public string inputFolder = "";
        public bool hidden = false;
        private DispatcherTimer searchTEMPsInputTimer;
        private DispatcherTimer loadingWindowTimer;
        public string currentThemeBrightness = "Dark";
        public TreeViewBackup treeViewBackup;
        public Message message;
        public List<TreeNode> visitedNodes;

        #region defined data types

        public class ZGuid
        {
            public string controlName;
            public string[] subControlNames;
            public string zguidString;
            public uint _a;
            public ushort _b;
            public ushort _c;
            public byte _d;
            public byte _e;
            public byte _f;
            public byte _g;
            public byte _h;
            public byte _i;
            public byte _j;
            public byte _k;
        }

        #endregion

        public delegate int execute_generate_rpkg_files_from_data(string outputFolder);

        public delegate int execute_export_map_data_to_folder(uint temps_index, string map_name, string output_path);

        public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        #region imported methods

        [DllImport("rpkg-lib.dll", EntryPoint = "clear_temp_tblu_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_temp_tblu_data();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_entries_with_logical_parent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_entries_with_logical_parent(uint temps_index, uint logical_parent);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_entries_with_logical_parent_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent_data();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_temp_entries", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_entries(uint temps_index, string value_type, string type_string);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_entries", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_entries(uint temps_index, uint entry_index, string value_type);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_entries_hash_reference_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_reference_data(uint temps_index, uint entry_index);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_entries_hash_references", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_references(uint temps_index, uint entry_index);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_enum_values", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_enum_values(uint temps_index, string property_type);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_temp_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_index(string temp_hash_string);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_top_level_logical_parents", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_top_level_logical_parents(uint temps_index);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_all_bricks", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_bricks(uint temps_index);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_response_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_response_string();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_temp_subentity_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_subentity_count(uint temps_index);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_tblu_subentity_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_tblu_subentity_count(uint temps_index);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_entry_name", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entry_name(uint temp_index, int entry_index);

        #endregion


    }
}

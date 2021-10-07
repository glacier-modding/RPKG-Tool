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
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading;
using MahApps.Metro.Controls;
using ControlzEx.Theming;
using System.Globalization;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Application = System.Windows.Application;
using Brushes = System.Windows.Media.Brushes;
using Button = System.Windows.Controls.Button;
using Color = System.Windows.Media.Color;
using ComboBox = System.Windows.Controls.ComboBox;
using Label = System.Windows.Controls.Label;
using ListBox = System.Windows.Forms.ListBox;
using TextBox = System.Windows.Controls.TextBox;
using TreeView = System.Windows.Controls.TreeView;

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
            if (currentThemeBrightness == "Dark")
            {
                MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
            }
            else if (currentThemeBrightness == "Light")
            {
                MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
                MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
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

            message.message.Content = "Loading Entity/Brick (TEMP/TBLU) Editor Treeview Nodes.\n\nThe GUI will seem frozen for a short time while the nodes are loaded.\n\nThe more recursive TEMP/TBLU files found, the longer the loading time will be.\n\nThis window will disappear once all the nodes have been loaded.";

            message.Topmost = true;

            message.Show();

            if (loadingWindowTimer == null)
            {
                loadingWindowTimer = new System.Windows.Threading.DispatcherTimer();

                loadingWindowTimer.Interval = TimeSpan.FromMilliseconds(400);

                loadingWindowTimer.Tick += LoadingWindowTimer_Tick;
            }

            loadingWindowTimer.Stop();
            loadingWindowTimer.Start();
        }

        private void LoadingWindowTimer_Tick(object sender, EventArgs e)
        {
            var timer = (sender as System.Windows.Threading.DispatcherTimer);

            if (timer == null)
            {
                return;
            }

            timer.Stop();

            LoadMainTreeView();
        }

        private void MetroWindow_Activated(object sender, EventArgs e)
        {
            if (hidden)
            {
                if (currentThemeBrightness == "Dark")
                {
                    MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                    MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
                }
                else if (currentThemeBrightness == "Light")
                {
                    MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
                    MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
                }

                LoadLoadingWindow();

                hidden = false;
            }
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
            //clear_temp_tblu_data();

            MainTreeView.Nodes.Clear();

            while (MainStackPanelTEMP.Children.Count > 0)
            {
                MainStackPanelTEMP.Children.RemoveAt(MainStackPanelTEMP.Children.Count - 1);
            }

            visitedNodes = null;
            visitedNodes = new List<System.Windows.Forms.TreeNode>();

            controls = null;
            controls = new List<string>();
            controlsChanged = null;
            controlsChanged = new List<bool>();
            controlsTEMPIndexes = null;
            controlsTEMPIndexes = new List<UInt32>();
            controlJSONPointers = null;
            controlJSONPointers = new List<string>();
            controlJSONPointersTypes = null;
            controlJSONPointersTypes = new List<string>();
            controlZGuids = null;
            controlZGuids = new List<ZGuid>();

            controlCount = 0;

            tempFilesChanged = null;
            tempFilesChanged = new List<UInt32>();

            MainTreeView.BeginUpdate();

            MainTreeView.AfterSelect += MainTreeView_AfterSelect;

            {
                var topItem = new System.Windows.Forms.TreeNode();

                topItem.Text = tempFileNameFull.Replace("(", "").Replace(")", "");

                string responseString = Marshal.PtrToStringAnsi(get_top_level_logical_parents(temps_index));

                //MessageBoxShow(responseString);

                string[] topLevelParents = responseString.Trim(',').Split(',');

                foreach (string topLevelParent in topLevelParents)
                {
                    UInt32 logical_parent = 0xFFFFFFFF;

                    UInt32.TryParse(topLevelParent, out logical_parent);

                    int entry_data_size = get_entries_with_logical_parent(temps_index, logical_parent);

                    byte[] entry_data = new byte[entry_data_size];

                    Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                    int data_pointer = 0;

                    while (data_pointer < entry_data_size)
                    {
                        UInt32 entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        UInt32 entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                        data_pointer += (int)entryNameLength;

                        //MessageBoxShow(entryIndex.ToString() + ", " + entryNameLength.ToString() + ", " + entryName);

                        if (MainTreeView.ImageList == null) LoadImageList();

                        var item = new System.Windows.Forms.TreeNode();

                        item.Text = entryName + " (" + entryIndex.ToString() + ")";
                        item.Name = entryName.ToString();
                        item.Tag = entryIndex.ToString() + "," + temps_index.ToString();
                        
                        LoadTreeView(ref item);

                        topItem.Nodes.Add(item);
                    }
                }

                int nodeCount = topItem.Nodes.Count;
                System.Windows.Forms.TreeNode[] nodes = new System.Windows.Forms.TreeNode[nodeCount];
                topItem.Nodes.CopyTo(nodes, 0);

                MainTreeView.Nodes.Add(topItem);
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

                        var topItem = new System.Windows.Forms.TreeNode();

                        topItem.Text = brick;

                        responseString = Marshal.PtrToStringAnsi(get_top_level_logical_parents((UInt32)temp_index_hash_reference));

                        //MessageBoxShow(responseString);

                        string[] topLevelParents = responseString.Trim(',').Split(',');

                        foreach (string topLevelParent in topLevelParents)
                        {
                            UInt32 logical_parent = 0xFFFFFFFF;

                            UInt32.TryParse(topLevelParent, out logical_parent);

                            int entry_data_size = get_entries_with_logical_parent((UInt32)temp_index_hash_reference, logical_parent);

                            byte[] entry_data = new byte[entry_data_size];

                            Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, entry_data_size);

                            int data_pointer = 0;

                            while (data_pointer < entry_data_size)
                            {
                                UInt32 entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                                data_pointer += 4;

                                UInt32 entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                                data_pointer += 4;

                                string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                                data_pointer += (int)entryNameLength;

                                //MessageBoxShow(entryIndex.ToString() + ", " + entryNameLength.ToString() + ", " + entryName);

                                if (MainTreeView.ImageList == null) LoadImageList();

                                var item = new System.Windows.Forms.TreeNode();

                                item.Text = entryName + " (" + entryIndex.ToString() + ")";
                                item.Name = entryName.ToString();
                                item.Tag = entryIndex.ToString() + "," + temp_index_hash_reference.ToString();
                                
                                

                                LoadTreeView(ref item);

                                topItem.Nodes.Add(item);
                            }
                        }

                        int nodeCount = topItem.Nodes.Count;
                        System.Windows.Forms.TreeNode[] nodes = new System.Windows.Forms.TreeNode[nodeCount];
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
                MessageBoxShow("Warning: There is a subEntities mismatch between the TEMP and TBLU files.\n\nThe TEMP file has " + temp_subentity_count.ToString() + " subEntities\n\nThe TBLU file has " + tblu_subentity_count.ToString() + " subEntities\n\nHaving mismatched subEntities (nodes in the treeview) counts can result in the nodes being misnamed.\n\nAlways look for the newest version of a TEMP file in the patch archives for a given chunk/dlc RPKG.");
            }
        }

        private void LoadTreeView(ref System.Windows.Forms.TreeNode masterTreeViewItem)
        {
            List<TreeNode> nodesToAdd = new List<TreeNode>();

            string entityName = masterTreeViewItem.Name;


            UInt32 temp_entryIndex = 0;

            string nodeData = masterTreeViewItem.Tag.ToString();

            UInt32.TryParse(nodeData.Split(',')[0], out temp_entryIndex);

            UInt32 temp_temp_index = 0;

            UInt32.TryParse(nodeData.Split(',')[1], out temp_temp_index);

            
            string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_references(temp_temp_index, temp_entryIndex));

            string[] hashReferences = hashReferenceData.Split(',');

            int temp_index_hash_reference = get_temp_index(hashReferences[0]);

            if (temp_index_hash_reference > 0)
            {
                UInt32 logical_parent = 0xFFFFFFFF;

                int hash_entry_data_size = get_entries_with_logical_parent((UInt32)temp_index_hash_reference, logical_parent);

                if (hash_entry_data_size > 0)
                {
                    byte[] entry_data = new byte[hash_entry_data_size];

                    Marshal.Copy(get_entries_with_logical_parent_data(), entry_data, 0, hash_entry_data_size);

                    int data_pointer = 0;

                    while (data_pointer < hash_entry_data_size)
                    {
                        UInt32 entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        UInt32 entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                        data_pointer += (int)entryNameLength;

                        //MessageBoxShow(entryIndex.ToString() + ", " + entryNameLength.ToString() + ", " + entryName);

                        var itemHashReference = new System.Windows.Forms.TreeNode();

                        itemHashReference.Text = entryName + " (" + entryIndex.ToString() + ") (" + temp_index_hash_reference.ToString() + ")";
                        itemHashReference.Name = entryName.ToString();
                        itemHashReference.Tag = entryIndex.ToString() + "," + temp_index_hash_reference.ToString();

                        LoadTreeView(ref itemHashReference);

                        //masterTreeViewItem.Nodes.Add(itemHashReference);
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
                    UInt32 entryIndex = BitConverter.ToUInt32(entry_data, data_pointer);
                    data_pointer += 4;

                    UInt32 entryNameLength = BitConverter.ToUInt32(entry_data, data_pointer);
                    data_pointer += 4;

                    string entryName = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryNameLength);
                    data_pointer += (int)entryNameLength;

                    //MessageBoxShow(entryIndex.ToString() + ", " + entryNameLength.ToString() + ", " + entryName);

                    var item2 = new System.Windows.Forms.TreeNode();

                    item2.Text = entryName + " (" + entryIndex.ToString() + ")";
                    item2.Name = entryName.ToString();
                    item2.Tag = entryIndex.ToString() + "," + temp_temp_index.ToString();

                    hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_references(temp_temp_index, entryIndex));
                    hashReferences = hashReferenceData.Split(',');

                    LoadTreeView(ref item2);

                    loadNodeIcon(ref item2, hashReferences[0]);
                    

                    //masterTreeViewItem.Nodes.Add(item2);
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

            string nodeName = (sender as Button).Tag.ToString();

            Message tempMessage = new Message();

            tempMessage.message.Content = "Locating " + nodeName + "...";

            tempMessage.Show();

            foreach (System.Windows.Forms.TreeNode node in MainTreeView.Nodes)
            {
                if (!foundNode)
                {
                    GoToNode(node, ref nodeName, ref foundNode);
                }
            }

            tempMessage.Close();
        }

        void GoToNode(System.Windows.Forms.TreeNode mainNode, ref string nodeName, ref bool foundNode)
        {
            foreach (System.Windows.Forms.TreeNode node in mainNode.Nodes)
            {
                if (!foundNode)
                {
                    GoToNode(node, ref nodeName, ref foundNode);

                    if (node.Text.StartsWith(nodeName))
                    {
                        foundNode = true;

                        MainTreeView.Focus();

                        MainTreeView.SelectedNode = node;

                        //MessageBoxShow(node.Text);
                    }

                    //if (foundNode)
                    //{
                    //node.Expand();

                    //MessageBoxShow(node.Text);
                    //}
                }
            }
        }

        private void UpdateTEMPFile()
        {
            if (controls != null)
            {
                if (controlZGuids.Count > 0)
                {
                    foreach (ZGuid zguid in controlZGuids)
                    {
                        DependencyObject descendant = FindDescendant(EditorWindow, zguid.controlName);

                        if (descendant != null)
                        {
                            if (descendant is TextBox)
                            {
                                if (ProcessZGuid(zguid.controlName, (descendant as TextBox).Text))
                                {
                                    DependencyObject descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[0]);
                                    (descendant2 as TextBox).Text = zguid._a.ToString().PadLeft(8, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[1]);
                                    (descendant2 as TextBox).Text = zguid._b.ToString().PadLeft(4, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[2]);
                                    (descendant2 as TextBox).Text = zguid._c.ToString().PadLeft(4, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[3]);
                                    (descendant2 as TextBox).Text = zguid._d.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[4]);
                                    (descendant2 as TextBox).Text = zguid._e.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[5]);
                                    (descendant2 as TextBox).Text = zguid._f.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[6]);
                                    (descendant2 as TextBox).Text = zguid._g.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[7]);
                                    (descendant2 as TextBox).Text = zguid._h.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[8]);
                                    (descendant2 as TextBox).Text = zguid._i.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[9]);
                                    (descendant2 as TextBox).Text = zguid._j.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);

                                    descendant2 = FindDescendant(EditorWindow, zguid.subControlNames[10]);
                                    (descendant2 as TextBox).Text = zguid._k.ToString().PadLeft(2, '0');
                                    TextBoxChanged((descendant2 as TextBox).Name);
                                }
                            }
                        }
                    }
                }

                bool update = false;

                foreach (bool controlChange in controlsChanged)
                {
                    if (controlChange)
                    {
                        update = true;
                    }
                }

                if (update)
                {
                    MemoryStream updateDataMemoryStream = new MemoryStream();
                    BinaryWriter updateDataWriter = new BinaryWriter(updateDataMemoryStream);

                    for (int i = 0; i < controls.Count; i++)
                    {
                        DependencyObject descendant = FindDescendant(EditorWindow, controls[i]);

                        if (descendant != null)
                        {
                            if (descendant is TextBox)
                            {
                                string value = (descendant as TextBox).Text;

                                if (value != "" && controlsChanged[i])
                                {
                                    //MessageBoxShow("Textbox: " + controlJSONPointers[i] + ": " + value);

                                    UInt32 controlJSONPointersLength = (UInt32)controlJSONPointers[i].Length;
                                    UInt32 controlJSONPointersTypesLength = (UInt32)controlJSONPointersTypes[i].Length;
                                    UInt32 valueLength = (UInt32)value.Length;

                                    updateDataWriter.Write(controlJSONPointersLength);
                                    updateDataWriter.Write(Encoding.UTF8.GetBytes(controlJSONPointers[i]));
                                    updateDataWriter.Write(controlJSONPointersTypesLength);
                                    updateDataWriter.Write(Encoding.UTF8.GetBytes(controlJSONPointersTypes[i]));
                                    updateDataWriter.Write(valueLength);
                                    updateDataWriter.Write(Encoding.UTF8.GetBytes(value));
                                }
                                else if (value == "" && controlJSONPointersTypes[i] != "ZString")
                                {
                                    MessageBoxShow("Error: The textbox for " + controlJSONPointers[i] + " is empty, can not proceed.");

                                    return;
                                }
                            }
                            else if (descendant is ComboBox)
                            {
                                string value = (descendant as ComboBox).SelectedItem.ToString();

                                if (value != "" && controlsChanged[i])
                                {
                                    //MessageBoxShow("Textbox: " + controlJSONPointers[i] + ": " + value + ", length: " + value.Length.ToString());

                                    UInt32 controlJSONPointersLength = (UInt32)controlJSONPointers[i].Length;
                                    UInt32 controlJSONPointersTypesLength = (UInt32)controlJSONPointersTypes[i].Length;
                                    UInt32 valueLength = (UInt32)value.Length;

                                    updateDataWriter.Write(controlJSONPointersLength);
                                    updateDataWriter.Write(Encoding.UTF8.GetBytes(controlJSONPointers[i]));
                                    updateDataWriter.Write(controlJSONPointersTypesLength);
                                    updateDataWriter.Write(Encoding.UTF8.GetBytes(controlJSONPointersTypes[i]));
                                    updateDataWriter.Write(valueLength);
                                    updateDataWriter.Write(Encoding.UTF8.GetBytes(value));
                                }
                                else if (value == "" && controlJSONPointersTypes[i] != "ZString")
                                {
                                    MessageBoxShow("Error: The combobox for " + controlJSONPointers[i] + " is empty, can not proceed.");

                                    return;
                                }
                            }
                        }
                    }

                    updateDataWriter.Flush();

                    byte[] updateData = updateDataMemoryStream.ToArray();

                    GCHandle updateDataHandle = GCHandle.Alloc(updateData, GCHandleType.Pinned);
                    try
                    {
                        IntPtr address = updateDataHandle.AddrOfPinnedObject();

                        int return_value = update_temp_file(temp_index, entity_index, address, (UInt32)updateData.Length);
                    }
                    finally
                    {
                        if (updateDataHandle.IsAllocated)
                        {
                            updateDataHandle.Free();
                        }
                    }

                    string responseString = Marshal.PtrToStringAnsi(get_response_string());

                    if (responseString != "")
                    {
                        MessageBoxShow(responseString);
                    }
                }
            }
        }

        private void PreviousNode_Click(object sender, RoutedEventArgs e)
        {
            if (visitedNodes.Count > 1)
            {
                System.Windows.Forms.TreeNode node;

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
        }

        private void MainTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            if (!hidden)
            {
                System.Windows.Forms.TreeNode item = (e.Node as System.Windows.Forms.TreeNode);

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

                string header = item.Text.ToString();

                if (MainTreeView.Nodes.Count > 0)
                {
                    UpdateTEMPFile();

                    textBoxesLoaded = false;

                    //MessageBoxShow(header);

                    while (MainStackPanelTEMP.Children.Count > 0)
                    {
                        MainStackPanelTEMP.Children.RemoveAt(MainStackPanelTEMP.Children.Count - 1);
                    }

                    controls = null;
                    controls = new List<string>();
                    controlsChanged = null;
                    controlsChanged = new List<bool>();
                    controlsTEMPIndexes = null;
                    controlsTEMPIndexes = new List<UInt32>();
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

                        temp_index = (UInt32)get_temp_index(hashFileName);

                        Label label1 = new Label();
                        label1.Content = hashFileName + "'s Data:";
                        label1.FontSize = 18;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        label1 = new Label();
                        label1.Content = "IOI String:";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        Label label2 = new Label();
                        label2.Content = ioiString;

                        MainStackPanelTEMP.Children.Add(label2);

                        label1 = new Label();
                        label1.Content = "Property data (subType):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput_TEMP("subType", "int32");

                        label1 = new Label();
                        label1.Content = "Property data (blueprintIndexInResourceHeader):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput_TEMP("blueprintIndexInResourceHeader", "int32");

                        label1 = new Label();
                        label1.Content = "Property data (rootEntityIndex):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput_TEMP("rootEntityIndex", "int32");

                        label1 = new Label();
                        label1.Content = "Property data (propertyOverrides):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput_TEMP("propertyOverrides", "TArray<SEntityTemplatePropertyOverride>");

                        label1 = new Label();
                        label1.Content = "Property data (externalSceneTypeIndicesInResourceHeader):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput_TEMP("externalSceneTypeIndicesInResourceHeader", "TArray<int32>");
                    }
                    else if (item.Text.Contains("("))
                    {

                        string entityName = item.Name;

                        string nodeData = item.Tag.ToString();

                        UInt32 entryIndex = 0;

                        UInt32.TryParse(nodeData.Split(',')[0], out entryIndex);

                        entity_index = entryIndex;

                        temp_index = 0;

                        UInt32.TryParse(nodeData.Split(',')[1], out temp_index);

                        string entryIndexString = nodeData.Split(',')[0];

                        //MessageBoxShow(entryIndexString);
                        //MessageBoxShow(temp_index.ToString());
                        //MessageBoxShow(entityName);

                        string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_reference_data(temp_index, entryIndex));

                        //MessageBoxShow(hashReferenceData);

                        Label label1 = new Label();
                        label1.Content = entityName + "'s Data:";
                        label1.FontSize = 18;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        label1 = new Label();
                        label1.Content = "Hash reference data:";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        Label label2 = new Label();
                        label2.Content = hashReferenceData;

                        MainStackPanelTEMP.Children.Add(label2);

                        label1 = new Label();
                        label1.Content = "Property data (propertyValues):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput(entryIndex, "propertyValues");

                        label1 = new Label();
                        label1.Content = "Property data (postInitPropertyValues):";
                        label1.FontSize = 16;
                        label1.FontWeight = FontWeights.Bold;

                        MainStackPanelTEMP.Children.Add(label1);

                        AppendInput(entryIndex, "postInitPropertyValues");

                        if (temp_file_version == 3)
                        {
                            label1 = new Label();
                            label1.Content = "Property data (platformSpecificPropertyValues):";
                            label1.FontSize = 16;
                            label1.FontWeight = FontWeights.Bold;

                            MainStackPanelTEMP.Children.Add(label1);

                            AppendInput(entryIndex, "platformSpecificPropertyValues");
                        }
                    }

                    textBoxesLoaded = true;
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
                        UInt32 entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

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

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

                        propertyValuePropertyIDsInput.Add(entryData);

                        if (added)
                        {
                            propertyValuePropertyIDs.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

                        propertyValueTypesInput.Add(entryData);

                        if (added)
                        {
                            propertyValueTypes.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

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

                    //string output = "";

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        for (int j = 0; j < propertyValuesInput.Count; j++)
                        {
                            string[] propertyValue = propertyValuesInput[j].Split(' ');

                            if (propertyValue.Length >= 3)
                            {
                                string propertyValuesString = valueType;

                                int position2 = propertyValue[0].IndexOf(propertyValuesString);

                                if (position2 >= 0)
                                {
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

                                    string debugString = propertyValuePropertyIDs[i] + "\n";
                                    debugString += propertyValueTypes[i] + "\n";
                                    debugString += propertyValues[i][propertyValueValNames[i].Count - 1] + "\n";
                                    debugString += propertyValueValNames[i][propertyValueValNames[i].Count - 1] + "\n";
                                    debugString += propertyValueJSONPointers[i][propertyValueJSONPointers[i].Count - 1] + "\n";
                                    debugString += propertyValueJSONPointersTypes[i][propertyValueJSONPointersTypes[i].Count - 1] + "\n";
                                    debugString += propertyValueVals[i][propertyValueVals[i].Count - 1] + "\n";

                                    //MessageBoxShow(debugString);
                                }
                            }
                            else
                            {
                                MessageBoxShow("Error: Property value string is malformed: " + propertyValues[j]);
                            }
                        }
                    }

                    //MessageBoxShow(output);

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        if (propertyValueVals[i].Count > 0)
                        {
                            string enumValues = Marshal.PtrToStringAnsi(get_enum_values(temp_index, propertyValueTypes[i]));

                            if (enumValues != "")
                            {
                                AppendInput_enum(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, ref enumValues);
                            }
                            else if (propertyValueTypes[i] == "bool")
                            {
                                AppendInput_bool(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else if (propertyValueTypes[i] == "ZGuid")
                            {
                                AppendInput_ZGuid(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else
                            {
                                AppendInput_Default(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                        }
                    }
                }
                else
                {
                    Label label2 = new Label();
                    label2.Content = "  None";

                    MainStackPanelTEMP.Children.Add(label2);
                }
            }
        }

        private void AppendInput(UInt32 entryIndex, string valueType)
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
                        UInt32 entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        string entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

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

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

                        propertyValuePropertyIDsInput.Add(entryData);

                        if (added)
                        {
                            propertyValuePropertyIDs.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

                        propertyValueTypesInput.Add(entryData);

                        if (added)
                        {
                            propertyValueTypes.Add(entryData);
                        }

                        entryDataLength = BitConverter.ToUInt32(entry_data, data_pointer);
                        data_pointer += 4;

                        entryData = Encoding.UTF8.GetString(entry_data, data_pointer, (int)entryDataLength);
                        data_pointer += (int)entryDataLength;

                        //MessageBoxShow(entryDataLength.ToString() + ", " + entryData);

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

                    //string output = "";

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        for (int j = 0; j < propertyValuesInput.Count; j++)
                        {
                            string[] propertyValue = propertyValuesInput[j].Split(' ');

                            if (propertyValue.Length >= 3)
                            {
                                string propertyValuesString = valueType + "/" + propertyValuesNumbers[i] + "/value/$val";

                                int position2 = propertyValue[0].IndexOf(propertyValuesString);

                                if (position2 >= 0)
                                {
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

                                    string debugString = propertyValuePropertyIDs[i] + "\n";
                                    debugString += propertyValueTypes[i] + "\n";
                                    debugString += propertyValues[i][propertyValueValNames[i].Count - 1] + "\n";
                                    debugString += propertyValueValNames[i][propertyValueValNames[i].Count - 1] + "\n";
                                    debugString += propertyValueJSONPointers[i][propertyValueJSONPointers[i].Count - 1] + "\n";
                                    debugString += propertyValueJSONPointersTypes[i][propertyValueJSONPointersTypes[i].Count - 1] + "\n";
                                    debugString += propertyValueVals[i][propertyValueVals[i].Count - 1] + "\n";

                                    //MessageBoxShow(debugString);
                                }
                            }
                            else
                            {
                                MessageBoxShow("Error: Property value string is malformed: " + propertyValues[j]);
                            }
                        }
                    }

                    //MessageBoxShow(output);

                    for (int i = 0; i < propertyValuesNumbers.Count; i++)
                    {
                        if (propertyValueVals[i].Count > 0)
                        {
                            string enumValues = Marshal.PtrToStringAnsi(get_enum_values(temp_index, propertyValueTypes[i]));

                            if (enumValues != "")
                            {
                                AppendInput_enum(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, ref enumValues);
                            }
                            else if (propertyValueTypes[i] == "bool")
                            {
                                AppendInput_bool(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else if (propertyValueTypes[i] == "SColorRGB")
                            {
                                AppendInput_SColorRGB(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, false);
                            }
                            else if (propertyValueTypes[i] == "SColorRGBA")
                            {
                                AppendInput_SColorRGB(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes, true);
                            }
                            else if (propertyValueTypes[i] == "SMatrix43")
                            {
                                AppendInput_SMatrix43(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else if (propertyValueTypes[i] == "SVector2")
                            {
                                AppendInput_SVector2(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else if (propertyValueTypes[i] == "SVector3")
                            {
                                AppendInput_SVector3(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else if (propertyValueTypes[i] == "SVector4")
                            {
                                AppendInput_SVector4(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else if (propertyValueTypes[i] == "ZGuid")
                            {
                                AppendInput_ZGuid(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                            else
                            {
                                AppendInput_Default(temp_index, i, ref propertyValuePropertyIDs, ref propertyValueTypes, ref propertyValueVals, ref propertyValueValNames, ref propertyValueJSONPointers, ref propertyValueJSONPointersTypes);
                            }
                        }
                    }
                }
                else
                {
                    Label label2 = new Label();
                    label2.Content = "  None";

                    MainStackPanelTEMP.Children.Add(label2);
                }
            }
        }

        #region build and append entries

        private void AppendInput_Default(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
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

                    
                    Label label = new Label();
                    label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
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
                        comboBox.SelectionChanged += ComboBox_SelectionChanged;
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
                        comboBox.SelectionChanged += ComboBox_SelectionChanged;
                        grid.Children.Add(comboBox);
                        Grid.SetRow(comboBox, rowCount);
                        Grid.SetColumn(comboBox, 1);
                        Grid.SetColumnSpan(comboBox, 3);
                    }
                    else
                    {
                        

                        if (propertyValueValNames[propertyIndex][i].Contains("/entityIndex"))
                        {
                            int temp_entity_index = 0;

                            int.TryParse(propertyValueVals[propertyIndex][i], out temp_entity_index);

                            string temp_entity_name = Marshal.PtrToStringAnsi(get_entry_name(temp_index, temp_entity_index));

                            if (temp_entity_name == "")
                            {
                                TextBox textBox = new TextBox();
                                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                                textBox.Text = propertyValueVals[propertyIndex][i];
                                textBox.Margin = new Thickness(4, 0, 4, 0);
                                textBox.TextChanged += TextBox_TextChanged;
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
                                textBox.TextChanged += TextBox_TextChanged;
                                grid.Children.Add(textBox);
                                Grid.SetRow(textBox, rowCount);
                                Grid.SetColumn(textBox, 1);

                                textBox = new TextBox();
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
                            textBox.TextChanged += TextBox_TextChanged;
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


                    Label label = new Label();
                    label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":";
                    grid.Children.Add(label);
                    Grid.SetRow(label, rowCount);
                    Grid.SetColumn(label, 0);

                    TextBox textBox = new TextBox();
                    textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                    textBox.Text = propertyValueVals[propertyIndex][i];
                    textBox.Margin = new Thickness(4, 0, 4, 0);
                    textBox.TextChanged += TextBox_TextChanged;
                    grid.Children.Add(textBox);
                    Grid.SetRow(textBox, rowCount);
                    Grid.SetColumn(textBox, 1);
                    Grid.SetColumnSpan(textBox, 3);

                    rowCount += 3;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_bool(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
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

                    Label label = new Label();
                    label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
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
                    comboBox.SelectionChanged += ComboBox_SelectionChanged;
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

                    Label label = new Label();
                    label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
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
                    comboBox.SelectionChanged += ComboBox_SelectionChanged;
                    grid.Children.Add(comboBox);
                    Grid.SetRow(comboBox, rowCount);
                    Grid.SetColumnSpan(comboBox, 2);
                    Grid.SetColumn(comboBox, 1);

                    rowCount += 3;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_enum(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes, ref string enumValues)
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



                    Label label = new Label();
                    label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
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
                    comboBox.SelectionChanged += ComboBox_SelectionChanged;
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

                    Label label = new Label();
                    label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":"; ;
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
                    comboBox.SelectionChanged += ComboBox_SelectionChanged;
                    grid.Children.Add(comboBox);
                    Grid.SetRow(comboBox, rowCount);
                    Grid.SetColumn(comboBox, 1);
                    Grid.SetColumnSpan(comboBox, 2);

                    rowCount += 3;
                }
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SColorRGB(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes, bool rgba_mode)
        {
            Label label1 = new Label();

            label1.Content = propertyValuePropertyIDs[propertyIndex].Replace("_", "__") + ":";

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
                Label label = new Label();
                label.Content = propertyValueValNames[propertyIndex][i].Replace("_", "__") + " (" + propertyValueJSONPointersTypes[propertyIndex][i] + "):";
                grid.Children.Add(label);
                Grid.SetRow(label, 1);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);

                colorPickerName += textBox.Name + "x";

                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.TextChanged += TextBox_TextChanged;
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

            MahApps.Metro.Controls.ColorCanvas colorCanvas = new ColorCanvas();
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

            if (rgba_mode)
            {
                Grid.SetColumnSpan(colorCanvas, 9);
            }
            else
            {
                Grid.SetColumnSpan(colorCanvas, 7);
            }

            MainStackPanelTEMP.Children.Add(grid);
        }

        private void AppendInput_SMatrix43(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            Label label1 = new Label();

            label1.Content = propertyValuePropertyIDs[propertyIndex].Replace("_", "__") + ":";

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
                Label label = new Label();
                label.Content = propertyValueValNames[propertyIndex][i].Replace("_", "__") + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.TextChanged += TextBox_TextChanged;
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

        private void AppendInput_SVector2(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            Label label1 = new Label();

            label1.Content = propertyValuePropertyIDs[propertyIndex].Replace("_", "__") + ":";

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
                Label label = new Label();
                label.Content = propertyValueValNames[propertyIndex][i].Replace("_", "__") + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.TextChanged += TextBox_TextChanged;
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

        private void AppendInput_SVector3(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            Label label1 = new Label();

            label1.Content = propertyValuePropertyIDs[propertyIndex].Replace("_", "__") + ":";

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
                Label label = new Label();
                label.Content = propertyValueValNames[propertyIndex][i].Replace("_", "__") + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.TextChanged += TextBox_TextChanged;
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

        private void AppendInput_SVector4(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
        {
            Label label1 = new Label();

            label1.Content = propertyValuePropertyIDs[propertyIndex].Replace("_", "__") + ":";

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
                Label label = new Label();
                label.Content = propertyValueValNames[propertyIndex][i].Replace("_", "__") + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, columnCount);

                columnCount++;

                TextBox textBox = new TextBox();
                textBox.Name = GetNewControlName(temp_index, propertyValueJSONPointers[propertyIndex][i], propertyValueJSONPointersTypes[propertyIndex][i]);
                textBox.Text = propertyValueVals[propertyIndex][i];
                textBox.Margin = new Thickness(4, 0, 4, 0);
                textBox.TextChanged += TextBox_TextChanged;
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

        private void AppendInput_ZGuid(UInt32 temp_index, int propertyIndex, ref List<string> propertyValuePropertyIDs, ref List<string> propertyValueTypes, ref List<string>[] propertyValueVals, ref List<string>[] propertyValueValNames, ref List<string>[] propertyValueJSONPointers, ref List<string>[] propertyValueJSONPointersTypes)
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

                Label label = new Label();
                label.Content = formatPropertyName(propertyValuePropertyIDs[propertyIndex]) + ":";
                grid.Children.Add(label);
                Grid.SetRow(label, rowCount);
                Grid.SetColumn(label, 0);

                TextBox textBox = new TextBox();
                textBox.Name = "ZGuid" + controlZGuids.Count.ToString();
                textBox.TextChanged += new TextChangedEventHandler(ZGuidTextBoxChanged);



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

        #region inputFormatFilters

        private void ZGuidTextBoxChanged(object sender, TextChangedEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            if (textBox != null)
            {
                string value = textBox.Text;
                var regex = new Regex(@"[({]?[a-fA-F0-9]{8}[-]?([a-fA-F0-9]{4}[-]?){3}[a-fA-F0-9]{12}[})]?");
                if (!regex.IsMatch(value))
                {
                    textBox.Foreground = Brushes.Firebrick;
                }
                else
                {
                    //because of the themes it gets difficult to find the correct foreground color.
                    //it will now use the same color as the filter textbox, since the filters color is unlikely to ever change.
                    textBox.Foreground = FilterTextBox.Foreground;
                }
            }
        }

        #endregion

        void TextBoxChanged(string controlName)
        {
            if (textBoxesLoaded)
            {
                string[] controlData = controlName.Split('_');

                UInt32 controlNumber = 0;

                UInt32.TryParse(controlData[1], out controlNumber);

                tempFilesChanged.Add(controlsTEMPIndexes[(int)controlNumber]);

                controlsChanged[(int)controlNumber] = true;

                //MessageBoxShow(controlsTEMPIndexes[(int)controlNumber].ToString());
            }
        }

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBoxChanged((sender as TextBox).Name);
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (textBoxesLoaded)
            {
                string[] controlData = (sender as ComboBox).Name.Split('_');

                UInt32 controlNumber = 0;

                UInt32.TryParse(controlData[1], out controlNumber);

                tempFilesChanged.Add(controlsTEMPIndexes[(int)controlNumber]);

                controlsChanged[(int)controlNumber] = true;

                //MessageBoxShow(controlsTEMPIndexes[(int)controlNumber].ToString());
            }
        }

        private bool ProcessZGuid(string controlName, string zguidString)
        {
            foreach (ZGuid zguid in controlZGuids)
            {
                if (zguid.controlName == controlName)
                {
                    string[] zguidData = zguidString.Split('-');

                    if (zguidData.Length == 5)
                    {
                        if (zguidData[0].Length == 8 && zguidData[1].Length == 4 && zguidData[2].Length == 4 && zguidData[3].Length == 4 && zguidData[4].Length == 12)
                        {
                            bool return_value = UInt32.TryParse(zguidData[0], System.Globalization.NumberStyles.HexNumber, null, out zguid._a);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[0] + " could not be converted to a UInt32 value.");
                                return false;
                            }

                            return_value = UInt16.TryParse(zguidData[1], System.Globalization.NumberStyles.HexNumber, null, out zguid._b);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[1] + " could not be converted to a UInt16 value.");
                                return false;
                            }

                            return_value = UInt16.TryParse(zguidData[2], System.Globalization.NumberStyles.HexNumber, null, out zguid._c);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[2] + " could not be converted to a UInt16 value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[3].Substring(0, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._d);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[3].Substring(0, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[3].Substring(2, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._e);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[3].Substring(2, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[4].Substring(0, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._f);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[4].Substring(0, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[4].Substring(2, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._g);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[4].Substring(2, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[4].Substring(4, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._h);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[4].Substring(4, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[4].Substring(6, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._i);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[4].Substring(6, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[4].Substring(8, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._j);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[4].Substring(8, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return_value = byte.TryParse(zguidData[4].Substring(10, 2), System.Globalization.NumberStyles.HexNumber, null, out zguid._k);

                            if (!return_value)
                            {
                                MessageBoxShow("Error: ZGuid value " + zguidData[4].Substring(10, 2) + " could not be converted to a byte value.");
                                return false;
                            }

                            return true;
                        }
                        else
                        {
                            MessageBoxShow("Error: ZGuid value is not 36 characters long and in the format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.\n\nWhere the Xs are in the range 0-9 and A-F.");
                            return false;
                        }
                    }
                    else
                    {
                        MessageBoxShow("Error: ZGuid value is not 36 characters long and in the format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.\n\nWhere the Xs are in the range 0-9 and A-F.");
                        return false;
                    }
                }
            }
            return false;
        }

        private string GenerateZGuid(ref List<string> zguidData)
        {
            string zguidString = "";

            ZGuid zguid = new ZGuid();

            bool return_value = UInt32.TryParse(zguidData[0], out zguid._a);
            zguidString += zguid._a.ToString("X2").PadLeft(8, '0') + "-";

            return_value = UInt16.TryParse(zguidData[1], out zguid._b);
            zguidString += zguid._b.ToString("X2").PadLeft(4, '0') + "-";

            return_value = UInt16.TryParse(zguidData[2], out zguid._c);
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



        public string GetNewControlName(UInt32 temp_index, string input, string type)
        {
            controlsTEMPIndexes.Add(temp_index);

            controlJSONPointers.Add(input);

            controlJSONPointersTypes.Add(type);

            controlsChanged.Add(false);

            string control = "c_" + controlCount.ToString();

            controls.Add(control);

            controlCount++;

            return control;
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

                temp_index = 0;

                UInt32.TryParse(header[header.Length - 1], out temp_index);

                string responseString = "";//Marshal.PtrToStringAnsi(get_entries_with_logical_parent(temp_index, entryIndex));

                string hashReferenceData = Marshal.PtrToStringAnsi(get_entries_hash_references(temp_index, entryIndex));

                string[] hashReferences = hashReferenceData.Split(',');

                int temp_index_hash_reference = get_temp_index(hashReferences[0]);

                if (temp_index_hash_reference > 0)
                {
                    UInt32 logical_parent = 0xFFFFFFFF;

                    string responseStringHashReference = "";//Marshal.PtrToStringAnsi(get_entries_with_logical_parent((UInt32)temp_index_hash_reference, logical_parent));

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

        #region generateMenuClickEvents

        private void GenerateTEMPFile_Click(object sender, RoutedEventArgs e)
        {
            UpdateTEMPFile();

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
            UpdateTEMPFile();

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

        private void GenerateTEMPJSONFiles_Click(object sender, RoutedEventArgs e)
        {
            UpdateTEMPFile();

            int changedCount = get_number_of_changed_temps();

            if (changedCount == 0)
            {
                MessageBoxShow("No TEMP files have been changed.");
            }
            else
            {
                int totalCount = get_total_numer_of_temps();

                MessageQuestion messageBox = new MessageQuestion();
                messageBox.message.Content = "A total of " + changedCount.ToString() + " out of " + totalCount.ToString() + " TEMP files have been changed.\n\nWould you like to continue generating TEMP JSON files for them all?";
                messageBox.ShowDialog();

                if (messageBox.buttonPressed == "OKButton")
                {
                    var folderDialog = new Ookii.Dialogs.Wpf.VistaFolderBrowserDialog();

                    folderDialog.Description = "Save Changes And Generate New TEMP JSON file(s):";

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

                        int return_value = generate_json_files_from_data(outputFolder);

                        if (return_value == 0)
                        {
                            MessageBoxShow("TEMP JSON file(s) successfully generated in: " + outputFolder);
                        }
                        else
                        {
                            MessageBoxShow("Error generating TEMP JSON file(s) in: " + outputFolder);
                        }
                    }
                }
                else if (messageBox.buttonPressed == "CancelButton")
                {

                }
            }
        }

        #endregion

        private void SearchTEMPsTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (searchTEMPsInputTimer == null)
            {
                searchTEMPsInputTimer = new System.Windows.Threading.DispatcherTimer();

                searchTEMPsInputTimer.Interval = TimeSpan.FromMilliseconds(600);

                searchTEMPsInputTimer.Tick += SearchTEMPsTextBox_TimerTimeout;
            }

            searchTEMPsInputTimer.Stop();
            searchTEMPsInputTimer.Tag = (sender as TextBox).Text;
            searchTEMPsInputTimer.Start();
        }

        private void SearchTEMPsTextBox_TimerTimeout(object sender, EventArgs e)
        {
            var timer = (sender as System.Windows.Threading.DispatcherTimer);

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

                foreach (System.Windows.Forms.TreeNode node in MainTreeView.Nodes)
                {
                    FilterNodes(node, ref filter, childrenVisibleCount);
                }
            }

            MainTreeView.EndUpdate();
        }

        #region treeView navigation
        private int FilterNodes(System.Windows.Forms.TreeNode parentNode, ref string filter, int childrenVisibleCount)
        {
            int currentChildrenVisibleCount = 0;

            if (parentNode != null)
            {
                if (parentNode.Nodes.Count > 0)
                {
                    foreach (System.Windows.Forms.TreeNode node in parentNode.Nodes)
                    {
                        currentChildrenVisibleCount += FilterNodes(node, ref filter, 0);
                    }
                }

                //if (parentNode.Text.ToLower().Contains("pier_area_entrance_stairs_a (9425)"))
                //{
                //MessageBoxShow("LOL!!!");
                //}

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
            if (nodePath.Length > 0)
            {
                if(nodePath.EndsWith(@"\")) nodePath = nodePath.Substring(0, nodePath.Length - 1);
                NodePathTextBox.Text = nodePath;
            }
        }

        public class TreeViewBackup : List<TreeViewBackup>
        {
            public System.Windows.Forms.TreeNode Parent { get; }
            public System.Windows.Forms.TreeNodeCollection Children { get; }

            public TreeViewBackup(System.Windows.Forms.TreeNodeCollection children, System.Windows.Forms.TreeNode parent = null)
            {
                Parent = parent;
                Children = children;
                AddRange(Children.Cast<System.Windows.Forms.TreeNode>().Select(child => new TreeViewBackup(child.Nodes, child)));
            }

            public void Restore()
            {
                Children.Clear();
                this.ForEach(clone => clone.Restore());
                Children.AddRange(this.Select(n => n.Parent).ToArray());
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

        private void ColorCanvas_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            ColorCanvas colorCanvas = (sender as ColorCanvas);

            string[] textBoxes = colorCanvas.Name.Replace("cc_", "").Split('x');

            DependencyObject descendant = FindDescendant(EditorWindow, textBoxes[0]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.R);

            descendant = FindDescendant(EditorWindow, textBoxes[1]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.G);

            descendant = FindDescendant(EditorWindow, textBoxes[2]);
            (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.B);

            if (textBoxes.Length == 5)
            {
                descendant = FindDescendant(EditorWindow, textBoxes[3]);
                (descendant as TextBox).Text = StringByteToStringFloat(colorCanvas.A);
            }
        }

        private void NodePathTextBox_OnTextChanged(object sender, TextChangedEventArgs e)
        {

        }

        #region util methods



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

            return temp_float.ToString(CultureInfo.InvariantCulture);
        }

        private string NodeToPath(TreeNode node)
        {
            string nodePath = "";

            while (node.Parent != null)
            {
                nodePath = node.Name + @"\" + nodePath;
                node = node.Parent;
            }

            return nodePath;
        }

        private string EntityIndexToNodePath(string entityID)
        {
            var items = MainTreeView.Nodes.Find(entityID, true);
            if (items.Length > 0) return NodeToPath(items[0]);
            else return entityID;
        }


        private string formatPropertyName(string propertyName)
        {

            string formattedPropertyName = propertyName;
            if (!propertyName.StartsWith("m_")) return formattedPropertyName;
            else formattedPropertyName = formattedPropertyName.Substring(2);

            if (!Char.IsUpper(formattedPropertyName, 0)) formattedPropertyName = formattedPropertyName.Substring(1);

            string temp_propertyName = formattedPropertyName;
            int found = 0;
            for (int i = 1; i < formattedPropertyName.Length - 1; i++)
            {

                if (Char.IsUpper(formattedPropertyName, i) && !Char.IsUpper(formattedPropertyName, i + 1))
                {
                    temp_propertyName = temp_propertyName.Insert(i + found, " ");
                    found++;
                }

            }

            formattedPropertyName = temp_propertyName;
            return formattedPropertyName;

        }

        #endregion

        public List<string> matrixStringList;
        public List<string> propertyNamesList;
        public List<string> gltfNamesList;
        public List<string> gltfParentList;
        public List<string> gltfNodeEntryList;
        public List<int> gltfGodotIndexList;
        public List<UInt32> tempFilesChanged;
        public List<string> controls;
        public List<bool> controlsChanged;
        public List<UInt32> controlsTEMPIndexes;
        public List<string> controlJSONPointers;
        public List<string> controlJSONPointersTypes;
        public List<ZGuid> controlZGuids;
        public int controlCount = 0;
        public UInt32 entity_index = 0;
        public UInt32 temp_index = 0;
        public UInt32 temps_index = 0;
        public int temp_file_version = 0;
        public string tempFileName = "";
        public string tbluFileName = "";
        public string rpkgFilePath = "";
        public string tempFileNameFull = "";
        public string inputFolder = "";
        public string outputFolder = "";
        public bool hidden = false;
        public bool textBoxesLoaded = false;
        private System.Windows.Threading.DispatcherTimer searchTEMPsInputTimer;
        private System.Windows.Threading.DispatcherTimer loadingWindowTimer;
        public string currentThemeBrightness = "Dark";
        public TreeViewBackup treeViewBackup;
        public Message message;
        public List<System.Windows.Forms.TreeNode> visitedNodes;

        #region defined data types

        public class ZGuid
        {
            public string controlName;
            public string[] subControlNames;
            public string zguidString;
            public UInt32 _a;
            public UInt16 _b;
            public UInt16 _c;
            public byte _d;
            public byte _e;
            public byte _f;
            public byte _g;
            public byte _h;
            public byte _i;
            public byte _j;
            public byte _k;
        }

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

        struct SEntityTemplateReference
        {

            public string entityID;
            public Int64 externalSceneIndex;
            public Int32 entityIndex;
            public string exposedEntity;

            public SEntityTemplateReference(string entityID, Int64 externalSceneIndex, Int32 entityIndex,
                string exposedEntity)
            {
                this.entityID = entityID;
                this.externalSceneIndex = externalSceneIndex;
                this.entityIndex = entityIndex;
                this.exposedEntity = exposedEntity;
            }


        }

        #endregion

        public delegate int execute_generate_rpkg_files_from_data(string outputFolder);

        public delegate int execute_export_map_data_to_folder(UInt32 temps_index, string map_name, string output_path);

        public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        #region imported methods

        [DllImport("rpkg.dll", EntryPoint = "task_execute", CallingConvention = CallingConvention.Cdecl)]
        public static extern int task_execute(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "reset_task_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_status();

        [DllImport("rpkg.dll", EntryPoint = "clear_temp_tblu_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_temp_tblu_data();

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_entries_with_logical_parent(UInt32 temps_index, UInt32 logical_parent);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent_string(UInt32 vector_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent_data();

        [DllImport("rpkg.dll", EntryPoint = "get_temp_entries", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_entries(UInt32 temps_index, string value_type, string type_string);

        [DllImport("rpkg.dll", EntryPoint = "get_entries", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_entries(UInt32 temps_index, UInt32 entry_index, string value_type);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_data();

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_reference_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_reference_data(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_references", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_references(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "update_temp_file", CallingConvention = CallingConvention.Cdecl)]
        public static extern int update_temp_file(UInt32 temps_index, UInt32 entry_index, IntPtr update_data, UInt32 update_data_size);

        [DllImport("rpkg.dll", EntryPoint = "generate_temp_file_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_temp_file_from_data(UInt32 temps_index, string temp_file_path);

        [DllImport("rpkg.dll", EntryPoint = "get_enum_values", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_enum_values(UInt32 temps_index, string property_type);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_index(string temp_hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_number_of_changed_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_number_of_changed_temps();

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

        [DllImport("rpkg.dll", EntryPoint = "search_temp_files", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_temp_files(UInt32 temps_index, string search_str, int max_results);

        [DllImport("rpkg.dll", EntryPoint = "get_search_temp_files", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_search_temp_files();

        [DllImport("rpkg.dll", EntryPoint = "get_response_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_response_string();

        [DllImport("rpkg.dll", EntryPoint = "get_temp_version", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_version(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_subentity_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_subentity_count(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_tblu_subentity_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_tblu_subentity_count(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entry_name", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entry_name(UInt32 temp_index, int entry_index);

        [DllImport("rpkg.dll", EntryPoint = "generate_json_files_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_json_files_from_data(string temp_path);

        #endregion


    }
}

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
using DiscordRPC;
using Button = System.Windows.Controls.Button;
using ICSharpCode.AvalonEdit.Highlighting;
using ICSharpCode.AvalonEdit.Highlighting.Xshd;
using System.Xml;

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
				Client.SetPresence(new RichPresence()
				{
					Details = details,
					State = state,
					Assets = new Assets()
					{
						LargeImageKey = "icon",
						LargeImageText = "RPKG Tool"
					},
					Timestamps = timestamp,
					Buttons = new DiscordRPC.Button[]
					{
						new DiscordRPC.Button() { Label = "Download RPKG Tool", Url = "https://notex.app" }
					}
				}); ;
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
						else if (LeftTabControl.SelectedIndex == 2)
						{
							SetDiscordStatus("REPO View", "");
						}
						else if (LeftTabControl.SelectedIndex == 4)
						{
							SetDiscordStatus("Search View", "");
						}
						else if (LeftTabControl.SelectedIndex == 6)
						{
							SetDiscordStatus("Deep Search View", "");
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
			Process.Start("https://discord.gg/6UDtuYhZP6");
		}

		public MainWindow()
		{
			InitializeComponent();

			ThemeManager.Current.ChangeTheme(rpkg.App.Current, ThemeManager.Current.AddTheme(RuntimeThemeGenerator.Current.GenerateRuntimeTheme("Light", Colors.DodgerBlue)));

			timestamp = Timestamps.Now;
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
				messageBox.message.Content = "Error: The hash list file (hash__list.txt) is missing.\n\nIt's necessary for several functions.\n\nClick OK to automatically download it.\n\nYou can also download it manually from https://hitmandb.notex.app/latest-hashes.7z and extract it to the same directory as this program.";
				messageBox.ShowDialog();

				//MessageBoxShow(messageBox.buttonPressed);

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

			int currentVersion = load_hash_list(AppDomain.CurrentDomain.BaseDirectory + "\\hash_list.txt");

			DownloadExtractionProgress downloadExtractionProgress = new DownloadExtractionProgress();
			downloadExtractionProgress.operation = (int)Progress.Operation.MASS_EXTRACT;
			downloadExtractionProgress.ProgressBar.IsIndeterminate = true;
			downloadExtractionProgress.message.Content = "Checking https://hitmandb.notex.app/version to see if a new hash list is available...";
			downloadExtractionProgress.ShowDialog();

			if (currentVersion < downloadExtractionProgress.currentVersionAvailable)
			{
				MessageQuestion messageBox = new MessageQuestion();
				messageBox.message.Content = "There is a new version of the hash list available.\n\nClick OK to automatically update to the latest version.";
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
			item.Text = "Alternatively you can select from the menus above for additional functions.";
			MainTreeView.Nodes.Add(item);
			item = new System.Windows.Forms.TreeNode();
			item.Text = "NOTE: Subsequent searches are faster than the first search, as the first search";
			item.Name = "deepsearch";
			DeepSearchEntitiesTreeView.Nodes.Add(item);
			item = new System.Windows.Forms.TreeNode();
			item.Text = "      loads/keeps RPKG's entities into JSONs in memory while searching them.";
			item.Name = "deepsearch";
			DeepSearchEntitiesTreeView.Nodes.Add(item);
			item = new System.Windows.Forms.TreeNode();
			item.Text = "NOTE: Make sure you load all RPKGs in your Hitman Runtime folder to ensure";
			item.Name = "deepsearch";
			REPOTreeView.Nodes.Add(item);
			item = new System.Windows.Forms.TreeNode();
			item.Text = "you are searching/accessing the latest REPO and ORES from the game!";
			item.Name = "deepsearch";
			REPOTreeView.Nodes.Add(item);

			MainTreeView.Name = "MainTreeView";
			SearchRPKGsTreeView.Name = "SearchRPKGsTreeView";
			SearchHashListTreeView.Name = "SearchHashListTreeView";
			DeepSearchEntitiesTreeView.Name = "DeepSearchEntitiesTreeView";
			DeepSearchLocalizationTreeView.Name = "DeepSearchLocalizationTreeView";
			REPOTreeView.Name = "REPOTreeView";

			MainTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
			SearchRPKGsTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
			DeepSearchEntitiesTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
			DeepSearchLocalizationTreeView.NodeMouseClick += MainTreeView_NodeMouseClick;
			REPOTreeView.NodeMouseClick += REPOTreeView_NodeMouseClick;
			MainTreeView.AfterSelect += MainTreeView_AfterSelect;
			SearchRPKGsTreeView.AfterSelect += MainTreeView_AfterSelect;
			DeepSearchEntitiesTreeView.AfterSelect += MainTreeView_AfterSelect;
			DeepSearchLocalizationTreeView.AfterSelect += MainTreeView_AfterSelect;
			REPOTreeView.AfterSelect += REPOTreeView_AfterSelect;
			SearchHashListTreeView.AfterSelect += SearchHashListTreeView_AfterSelect;
			LeftTabControl.SelectionChanged += LeftTabControl_SelectionChanged;
			RightTabControl.SelectionChanged += RightTabControl_SelectionChanged;

			enumDictionary = new Dictionary<string, string[]>();
			enumDictionary.Add("BurstPattern", new string[] { "eEBP_None", "eEBP_Circular3Rings", "eEBP_Box" });
			enumDictionary.Add("SilenceRating", new string[] { "eSR_NotSilenced", "eSR_Silenced", "eSR_SuperSilenced", "eSR_CompletelySilenced" });
			enumDictionary.Add("AudioFireType", new string[] { "eWBC_AudioFireType_Single", "eWBC_AudioFireType_Full_Automatic" });
			enumDictionary.Add("AudioHeadType", new string[] { "eWBC_AudioHeadTailType_Silenced", "eWBC_AudioHeadTailType_Normal", "eWBC_AudioHeadTailType_SilencedSweetener", "eWBC_AudioHeadTailType_NormalSweetener" });
			enumDictionary.Add("AudioTailType", new string[] { "eWBC_AudioHeadTailType_Silenced", "eWBC_AudioHeadTailType_Normal", "eWBC_AudioHeadTailType_DryFire", "eWBC_AudioHeadTailType_SilencedSweetener", "eWBC_AudioHeadTailType_NormalSweetener" });
			enumDictionary.Add("AudioWeaponClass", new string[] { "eWBC_AudioClass_Pistol", "eWBC_AudioClass_SniperRifle", "eWBC_AudioClass_AssaultRifle", "eWBC_AudioClass_Shotgun", "eWBC_AudioClass_SMG" });
			enumDictionary.Add("AudioWeaponFamily", new string[] { "eWBC_AudioFamily_Heavy", "eWBC_AudioFamily_Standard", "eWBC_AudioFamily_Light", "eWBC_AudioFamily_NPC", "eWBC_AudioFamily_Exotics" });
			enumDictionary.Add("CheatGroup", new string[] { "eCGNone", "eCGDevices", "eCGPistols", "eCGSniper", "eCGAssaultRifles", "eCGExotics", "eCGSMGs", "eCGShotguns" });
			enumDictionary.Add("CarryAnimSetType", new string[] { "EAST_SUITCASE", "EAST_CARRY_WEAPON_ITEM", "EAST_SMALLOBJ", "EAST_CARRY_PROXY_MINE", "EAST_CARRY_1HANDEDWEAPON", "EAST_UNDEFINED", "EAST_CARRY_2HANDEDWEAPON", "EAST_CARRY_LEAD_PIPES", "EAST_PLATE", "EAST_GLASS", "EAST_BOX", "EAST_FLASHLIGHT", "EAST_CARRY_BRICK_MINE", "EAST_TRAY", "EAST_CARRY_UMBRELLA", "EAST_DEFAULT", "EAST_CARRY_TUBE", "EAST_WHISKYGLASS", "EAST_PHONE" });
			enumDictionary.Add("HolsterAbility", new string[] { "eHolsterQuestStorage", "eHolsteringAllowed", "eUndecided", "eHolsterTemporarilyOnly", "eCanNotBeHolstered", "eHolsterSecondaryOnly" });
			enumDictionary.Add("InventoryCategoryIcon", new string[] { "QuestItem", "melee", "key", "explosives", "questitem", "tool", "pistol", "INVALID_CATEGORY_ICON", "sniperrifle", "assaultrifle", "remote", "shotgun", "suitcase", "smg", "distraction", "poison", "Container", "Tool" });
			enumDictionary.Add("InventoryCategoryName", new string[] { "QuestItem", "melee", "key", "explosive", "", "tool", "pistol", "sniperrifle", "assaultrifle", "remote", "shotgun", "suitcase", "smg", "distraction", "poison", "USBStick", "Suitcase" });
			enumDictionary.Add("ItemHandsIdle", new string[] { "IH_ONEHANDED", "IH_TWOHANDED", "IH_NONE" });
			enumDictionary.Add("ItemHandsUse", new string[] { "IH_ONEHANDED", "IH_TWOHANDED", "IH_NONE" });
			enumDictionary.Add("ItemType", new string[] { "eOther_GenericPickup", "eCC_Brick", "eOther_Keycard_A", "eCC_RemoteExplosive", "eUnknownItemType", "eGun_HardBaller_01", "eItemAmmo", "eSniper_SakoTRG", "eRifle_HK416", "eDetonator", "eShotgun_M500New", "eSuitcase", "eCC_FireExtinguisher_01", "eCC_BaseballBat", "eSniper_SakoTRGSilenced", "eSMG_HK_UMP", "eShotgun_M500", "eCC_Knife", "eCC_Bottle", "eCC_Katana", "eCC_Screwdriver", "eCC_FiberWire", "eSyringe_Emetic", "eGun_Taurus24_7", "eCC_Cleaver", "eSyringe_Lethal", "eGun_44AutoMag", "eCC_Axe", "eShotgun_Spas12", "eCC_Radio", "cCC_Book_A", "eCC_C4Brick", "eCC_Hammer", "eCC_Wrench", "eCC_PoliceBaton", "eCC_MetalPipe", "eCC_PetrolCan", "eSMG_HK_UMPSilenced", "eCC_Bong", "eSyringe_Sedative", "eOther_Camera" });
			enumDictionary.Add("MainAnimSetType", new string[] { "EAST_UNDEFINED", "EAST_SUITCASE", "EAST_CARRY_LEAD_PIPES", "EAST_SMG", "EAST_ASSULT", "EAST_SHOTGUN", "EAST_PISTOL", "EAST_SMALLOBJ", "EAST_DEFAULT", "EAST_CARRY_1HANDEDWEAPON", "EAST_CARRY_UMBRELLA", "EAST_CARRY_BRICK_MINE" });
			enumDictionary.Add("OnlineTraits", new string[] { "NONE", "melee_nonlethal", "throw_nonlethal_deprecated", "Setpiece", "activator", "container", "explosive", "explosive_device", "Trap", "poison", "body", "pistol", "suspended", "sniperrifle", "assaultrifle", "detonator_explosive_device", "shotgun", "case", "distraction", "smg", "electricity", "tool", "melee_lethal", "throw_lethal_deprecated", "fiberwire", "syringe_poison", "consumable_poison", "melee", "questitem", "accident_explosion", "onlevel" });
			enumDictionary.Add("PlaceableType", new string[] { "PLACEABLE_NOT", "PLACEABLE_SLEEP_PHYSICS", "PLACEABLE_ATTACH", "PLACEABLE_FULL_PHYSICS", "PLACEABLE_NO_PHYSICS" });
			enumDictionary.Add("PoisonType", new string[] { "POISONTYPE_NONE", "POISONTYPE_SEDATIVE", "POISONTYPE_EMETIC", "POISONTYPE_LETHAL" });
			enumDictionary.Add("ActionRewardType", new string[] { "AR_QuestItem", "AR_None", "AR_Key", "AR_Keycard" });
			enumDictionary.Add("EquipAbility", new string[] { "EA_CANNOT_BE_EQUIPPED", "EA_EQUIP_IN_HANDS" });
			enumDictionary.Add("ItemHandsCoverAnimLayer", new string[] { "IH_NONE", "IH_ONEHANDED", "IH_TWOHANDED" });
			enumDictionary.Add("ItemHUDType", new string[] { "EIHT_None", "EIHT_GhostItem" });
			enumDictionary.Add("ItemSize", new string[] { "ITEMSIZE_SMALL", "ITEMSIZE_LARGE", "ITEMSIZE_MEDIUM" });
			enumDictionary.Add("MeleeDamageBehavior", new string[] { "EIMDB_Undefined", "EIMDB_Sticking", "EIMDB_Slashing" });
			enumDictionary.Add("Rarity", new string[] { "ITEMRARITY_COMMON", "ITEMRARITY_UNCOMMON", "ITEMRARITY_RARE" });
			enumDictionary.Add("ThrownCollisionLoudness", new string[] { "eLoudness_Default", "eLoudness_Normal", "eLoudness_Low", "eLoudness_Loud" });
			enumDictionary.Add("ThrowType", new string[] { "THROW_NONE", "THROW_PACIFY_LIGHT", "THROW_PACIFY_HEAVY", "THROW_KNOCKDOWN_LIGHT", "THROW_NORMAL", "THROW_DEADLY_LIGHT", "THROW_DEADLY_HEAVY", "THROW_COIN" });
			enumDictionary.Add("GripAnimType", new string[] { "IGT_Suitcase", "IGT_Melee_1H_Rock", "IGT_Gadget_Coin", "IGT_Gadget_C4", "IGT_Melee_1H_Crowbar", "IGT_Gadget_Mine", "IGT_Firearm_Pistol", "IGT_Firearm_SniperRifle", "IGT_Firearm_AssaultRifle_Bullpup", "IGT_Gadget_Detonator", "IGT_Gadget_Remote", "IGT_Melee_1H_FireExtinguisher", "IGT_Firearm_Shotgun", "IGT_Melee_2H_Stick", "IGT_Firearm_SMG", "IGT_Prop_1H", "IGT_Firearm_AssaultRifle_Carbine", "IGT_Prop_1H_Duck", "IGT_Melee_1H_Screwdriver", "IGT_None", "IGT_Melee_1H_Baton", "IGT_Melee_1H_Knife", "IGT_Melee_2H_Sword", "IGT_Gadget_Fiberwire", "IGT_Gadget_Vial", "IGT_Melee_1H_Syringe", "IGT_Firearm_Pistol_Stealth", "IGT_Melee_1H_Stick", "IGT_Melee_1H_Cleaver", "IGT_Prop_1h_Phone", "IGT_Firearm_Dartgun", "IGT_Melee_2H_Axe", "IGT_Firearm_SMG02", "IGT_Melee_1H_Sword", "IGT_Melee_1H_Hammer" });
			enumDictionary.Add("AmmoType", new string[] { "eAmmoGun", "eAmmoShotgun", "eAmmoSMG", "eAmmoSniper", "eAmmoRifle", "eAmmoDartTranquilizer", "eAmmoLightPistol", "eAmmoShotgunBeanbag", "eAmmoHarmless" });

			LoadResources();
		}

		private void MainTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			if (oneOrMoreRPKGsHaveBeenImported)
			{
				if (MainTreeView.Nodes.Count > 0)
				{
					System.Windows.Forms.TreeNode item = (e.Node as System.Windows.Forms.TreeNode);

					if (item == null)
					{
						return;
					}

					string itemHeader = item.Text;

					string itemName = item.Name;

					currentNodeText = item.Text;

					//MessageBoxShow(itemHeader);

					rpkgFilePath = GetRootTreeNode(e.Node).Text;

					if (itemName == "deepsearch")
					{
						DetailsTextBox.Text = itemHeader;

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

						if (ThirdTabRight.Visibility == Visibility.Visible)
						{
							ThirdTabRight.Visibility = Visibility.Collapsed;
							ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						return;
					}
					else if (itemHeader.Length == 4)
					{
						string resourceType = itemHeader;

						string rpkgFile = itemHeader.Substring(itemHeader.LastIndexOf("\\") + 1);

						if (LeftTabControl.SelectedIndex == 0)
						{
							SetDiscordStatus("Resource View", rpkgFile);
						}
						else if (LeftTabControl.SelectedIndex == 2)
						{
							SetDiscordStatus("REPO View", rpkgFile);
						}
						else if (LeftTabControl.SelectedIndex == 4)
						{
							SetDiscordStatus("Search View", rpkgFile);
						}
						else if (LeftTabControl.SelectedIndex == 6)
						{
							SetDiscordStatus("Deep Search View", rpkgFile);
						}

						UInt32 hashBasedOnResourceTypeCount = get_hash_based_on_resource_type_count(rpkgFilePath, resourceType);

						UInt64 resourceTypesDataSize = get_resource_types_data_size(rpkgFilePath, resourceType);

						DetailsTextBox.Text = rpkgFile + " has " + hashBasedOnResourceTypeCount.ToString("N0") + " hash files/resources of resource type " + resourceType + ",\n";
						DetailsTextBox.Text += "having a combined total data size of " + resourceTypesDataSize.ToString("N0") + " bytes";

						HexViewerTextBox.Text = "";

						if (ThirdTabRight.Visibility == Visibility.Visible)
						{
							ThirdTabRight.Visibility = Visibility.Collapsed;
							ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						if (FourthTabRight.Visibility == Visibility.Visible)
						{
							FourthTabRight.Visibility = Visibility.Collapsed;
							FourthTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						if (FifthTabRight.Visibility == Visibility.Visible)
						{
							FifthTabRight.Visibility = Visibility.Collapsed;
							FifthTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						if (SixthTabRight.Visibility == Visibility.Visible)
						{
							SixthTabRight.Visibility = Visibility.Collapsed;
							SixthTabRightSeparator.Visibility = Visibility.Collapsed;
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

						if (LeftTabControl.SelectedIndex == 0)
						{
							SetDiscordStatus("Resource View", rpkgFile);
						}
						else if (LeftTabControl.SelectedIndex == 2)
						{
							SetDiscordStatus("REPO View", rpkgFile);
						}
						else if (LeftTabControl.SelectedIndex == 4)
						{
							SetDiscordStatus("Search View", rpkgFile);
						}
						else if (LeftTabControl.SelectedIndex == 6)
						{
							SetDiscordStatus("Deep Search View", rpkgFile);
						}

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
							ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						if (FourthTabRight.Visibility == Visibility.Visible)
						{
							FourthTabRight.Visibility = Visibility.Collapsed;
							FourthTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						if (FifthTabRight.Visibility == Visibility.Visible)
						{
							FifthTabRight.Visibility = Visibility.Collapsed;
							FifthTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						if (SixthTabRight.Visibility == Visibility.Visible)
						{
							SixthTabRight.Visibility = Visibility.Collapsed;
							SixthTabRightSeparator.Visibility = Visibility.Collapsed;
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

						if (LeftTabControl.SelectedIndex == 0)
						{
							SetDiscordStatus("Resource View", hashFile);
						}
						else if (LeftTabControl.SelectedIndex == 2)
						{
							SetDiscordStatus("REPO View", hashFile);
						}
						else if (LeftTabControl.SelectedIndex == 4)
						{
							SetDiscordStatus("Search View", hashFile);
						}
						else if (LeftTabControl.SelectedIndex == 6)
						{
							SetDiscordStatus("Deep Search View", hashFile);
						}

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

						string currentRPKGFilePath = rpkgFilePath;

						currentHash = header[0];

						if (RightTabControl.SelectedIndex == 2)
						{
							LoadHexEditor();
						}

						if (RightTabControl.SelectedIndex == 6 && resourceType == "TEXT")
						{
							LoadImageViewer_TEXT(hashFile, hash);
						}

						if (RightTabControl.SelectedIndex == 6 && resourceType == "GFXI")
						{
							LoadImageViewer_GFXI(hash);
						}

						if (RightTabControl.SelectedIndex == 10 && resourceType == "PRIM")
						{
							LoadModelViewer(hash);
						}

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
									ThirdTabRightSeparator.Visibility = Visibility.Visible;
								}
							}
							else
							{
								if (ThirdTabRight.Visibility == Visibility.Visible)
								{
									ThirdTabRight.Visibility = Visibility.Collapsed;
									ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
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
									ThirdTabRightSeparator.Visibility = Visibility.Visible;
								}
							}
							else
							{
								if (ThirdTabRight.Visibility == Visibility.Visible)
								{
									ThirdTabRight.Visibility = Visibility.Collapsed;
									ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
								}
							}
						}
						else if (resourceType == "LINE")
						{
							string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

							if (!runtimeDirLoaded)
							{
								if (runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
								{
									ImportRPKGFileFolder(runtimeDirectory);
								}

								runtimeDirLoaded = true;
							}

							UInt32 localization_data_size = generate_localization_line_string(runtimeDirectory, hash, resourceType);

							byte[] localization_data = new byte[localization_data_size];

							Marshal.Copy(get_localization_line_string(), localization_data, 0, (int)localization_data_size);

							if (localization_data_size > 0)
							{
								DetailsTextBox.Text += Encoding.UTF8.GetString(localization_data);
							}
						}
						else
						{
							if (ThirdTabRight.Visibility == Visibility.Visible)
							{
								ThirdTabRight.Visibility = Visibility.Collapsed;
								ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
							}

							if (FourthTabRight.Visibility == Visibility.Visible)
							{
								FourthTabRight.Visibility = Visibility.Collapsed;
								FourthTabRightSeparator.Visibility = Visibility.Collapsed;
							}

							if (FifthTabRight.Visibility == Visibility.Visible)
							{
								FifthTabRight.Visibility = Visibility.Collapsed;
								FifthTabRightSeparator.Visibility = Visibility.Collapsed;
							}

							if (SixthTabRight.Visibility == Visibility.Visible)
							{
								SixthTabRight.Visibility = Visibility.Collapsed;
								SixthTabRightSeparator.Visibility = Visibility.Collapsed;
							}
						}

						int return_value = clear_hash_data_vector();

						if (resourceType == "PRIM")
						{
							if (SixthTabRight.Visibility == Visibility.Collapsed)
							{
								SixthTabRight.Visibility = Visibility.Visible;
								SixthTabRightSeparator.Visibility = Visibility.Visible;
							}
						}

						if (resourceType == "GFXI")
						{
							if (FourthTabRight.Visibility == Visibility.Collapsed)
							{
								FourthTabRight.Visibility = Visibility.Visible;
								FourthTabRightSeparator.Visibility = Visibility.Visible;
							}
						}

						if (resourceType == "TEXT")
						{
							if (FourthTabRight.Visibility == Visibility.Collapsed)
							{
								FourthTabRight.Visibility = Visibility.Visible;
								FourthTabRightSeparator.Visibility = Visibility.Visible;
							}
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
								FifthTabRightSeparator.Visibility = Visibility.Visible;
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
									FifthTabRightSeparator.Visibility = Visibility.Visible;
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
										FifthTabRightSeparator.Visibility = Visibility.Collapsed;
									}

									FirstTabRight.IsSelected = true;
								}
							}
							else
							{
								if (FifthTabRight.Visibility == Visibility.Visible)
								{
									FifthTabRight.Visibility = Visibility.Collapsed;
									FifthTabRightSeparator.Visibility = Visibility.Collapsed;
								}

								FirstTabRight.IsSelected = true;
							}
						}
					}

					//GC.Collect();
				}
			}
		}

		private void MainTreeView_AfterExpand(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			var item = (e.Node as System.Windows.Forms.TreeNode);

			if (item.Nodes.Count == 1 && item.Nodes[0].Text == "")
			{
				item.Nodes.Clear();

				string[] header = (item.Text as string).Split(' ');

				string resourceType = header[0];

				rpkgFilePath = GetRootTreeNode(e.Node).Text;

				UInt32 hashBasedOnResourceTypeCount = get_hash_based_on_resource_type_count(rpkgFilePath, resourceType);

				MainTreeView.BeginUpdate();

				int nodeCount = (int)hashBasedOnResourceTypeCount;

				System.Windows.Forms.TreeNode[] nodes = new System.Windows.Forms.TreeNode[nodeCount];

				for (UInt32 i = 0; i < hashBasedOnResourceTypeCount; i++)
				{
					string hash = "";

					hash = Marshal.PtrToStringAnsi(get_hash_based_on_resource_type_at(rpkgFilePath, resourceType, i));

					string[] temp_test = hash.Split('.');

					string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

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
			if (e.Button == System.Windows.Forms.MouseButtons.Right)
			{
				MainTreeView.SelectedNode = e.Node;

				//MessageBoxShow(e.Node.Text);

				//MessageBoxShow(e.Button.ToString());

				string header = e.Node.Text;

				string headerName = e.Node.Name;

				if (oneOrMoreRPKGsHaveBeenImported)
				{
					if (header.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
					{
						rpkgFilePath = header;

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

						SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

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

							SearchRPKGsTreeView.Nodes.Clear();

							bool treeview_item_found = false;

							int count = 0;

							int treeview_item_index = 0;

							foreach (System.Windows.Forms.TreeNode item in MainTreeView.Nodes)
							{
								if (item.Text.ToString() == rpkgFilePath)
								{
									treeview_item_index = count;

									treeview_item_found = true;
								}

								count++;
							}

							if (treeview_item_found)
							{
								MainTreeView.Nodes.RemoveAt(treeview_item_index);

								resetDeepSearchComboBoxes();

								//LoadREPO();

								//ImportRPKGFile(rpkgFilePath);

								//foreach (System.Windows.Forms.TreeNode item in MainTreeView.Nodes)
								//{
								//if (item.Text.ToString() == rpkgFilePath)
								//{
								//MainTreeView.SelectedNode = item;
								//}
								//}
							}
							else
							{
								MessageBoxShow("Error: Cound not find the treeview item for unloading for RPKG: " + rpkgFilePath);
							}

							return;
						}
						else
						{
							return;
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
					else if (headerName == "deepsearch")
					{
						DetailsTextBox.Text = header;

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

						if (ThirdTabRight.Visibility == Visibility.Visible)
						{
							ThirdTabRight.Visibility = Visibility.Collapsed;
							ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
						}

						return;
					}
					else if (header.Length == 4)
					{
						rpkgFilePath = GetRootTreeNode(e.Node).Text;

						string command = "";
						string input_path = rpkgFilePath;
						string filter = "";
						string search = "";
						string search_type = "";
						string output_path = userSettings.OutputFolder;

						Progress progress = new Progress();

						progress.operation = (int)Progress.Operation.MASS_EXTRACT;

						if (header == "PRIM")
						{
							string[] buttons = { "Extract All " + header, "Extract All " + header + " To GLB", "Cancel" };

							RightClickMenu rightClickMenu = new RightClickMenu(buttons);

							SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

							rightClickMenu.ShowDialog();

							//MessageBoxShow(rightClickMenu.buttonPressed);
							//MessageBoxShow(rpkgFilePath);

							if (rightClickMenu.buttonPressed == "button0")
							{
								command = "-extract_from_rpkg";

								filter = header;

								progress.message.Content = "Extracting All " + header + "...";
							}
							else if (rightClickMenu.buttonPressed == "button1")
							{
								command = "-extract_all_prim_to_glb_from";

								progress.message.Content = "Extracting All " + header + " To GLB...";

								string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + header + " To:", "");

								if (temp_outputFolder == "")
								{
									return;
								}

								output_path = temp_outputFolder;
							}
							else
							{
								return;
							}
						}
						else if (header == "TEXT")
						{
							string[] buttons = { "Extract All " + header, "Extract All " + header + " To TGA", "Cancel" };

							RightClickMenu rightClickMenu = new RightClickMenu(buttons);

							SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

							rightClickMenu.ShowDialog();

							//MessageBoxShow(rightClickMenu.buttonPressed);
							//MessageBoxShow(rpkgFilePath);

							if (rightClickMenu.buttonPressed == "button0")
							{
								command = "-extract_from_rpkg";

								filter = header;

								progress.message.Content = "Extracting All " + header + "...";
							}
							else if (rightClickMenu.buttonPressed == "button1")
							{
								command = "-extract_all_text_from";

								progress.message.Content = "Extracting All " + header + " To TGA...";

								string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + header + " To:", "");

								if (temp_outputFolder == "")
								{
									return;
								}

								output_path = temp_outputFolder;
							}
							else
							{
								return;
							}
						}
						else if (header == "LOCR" || header == "DLGE" || header == "RTLV")
						{
							string[] buttons = { "Extract All " + header, "Extract All " + header + " To JSON", "Cancel" };

							RightClickMenu rightClickMenu = new RightClickMenu(buttons);

							SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

							rightClickMenu.ShowDialog();

							//MessageBoxShow(rightClickMenu.buttonPressed);
							//MessageBoxShow(rpkgFilePath);

							if (rightClickMenu.buttonPressed == "button0")
							{
								command = "-extract_from_rpkg";

								filter = header;

								progress.message.Content = "Extracting All " + header + "...";
							}
							else if (rightClickMenu.buttonPressed == "button1")
							{
								if (header == "LOCR")
								{
									command = "-extract_locr_to_json_from";

									progress.message.Content = "Extracting All " + header + " To JSON...";
								}
								else if (header == "DLGE")
								{
									command = "-extract_dlge_to_json_from";

									progress.message.Content = "Extracting All " + header + " To JSON...";
								}
								else if (header == "RTLV")
								{
									command = "-extract_rtlv_to_json_from";

									progress.message.Content = "Extracting All " + header + " To JSON...";
								}
							}
							else
							{
								return;
							}
						}
						else if (header == "WWEM" || header == "WWES" || header == "WWEV" || header == "ORES" || header == "GFXI" || header == "JSON")
						{
							string buttonContent = "";

							if (header == "WWEM" || header == "WWES" || header == "WWEV")
							{
								buttonContent = "Extract All " + header + " To OGG To IOI Paths";
							}
							else if (header == "ORES")
							{
								buttonContent = "Extract All " + header + " To IOI Paths";
							}
							else if (header == "GFXI")
							{
								buttonContent = "Extract All " + header + " To Images To IOI Paths";
							}
							else if (header == "JSON")
							{
								buttonContent = "Extract All " + header + " To JSONs To IOI Paths";
							}

							string[] buttons = { "Extract All " + header, buttonContent, "Cancel" };

							RightClickMenu rightClickMenu = new RightClickMenu(buttons);

							SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

							rightClickMenu.ShowDialog();

							//MessageBoxShow(rightClickMenu.buttonPressed);
							//MessageBoxShow(rpkgFilePath);

							if (rightClickMenu.buttonPressed == "button0")
							{
								progress.ProgressBar.IsIndeterminate = true;

								command = "-extract_from_rpkg";

								filter = header;

								progress.message.Content = "Extracting All " + header + "...";
							}
							else if (rightClickMenu.buttonPressed == "button1")
							{
								if (header == "WWEM" || header == "WWES" || header == "WWEV" || header == "ORES")
								{
									Filter filterDialog = new Filter();

									filterDialog.message1.Content = "Enter extraction filter for " + header + " below.";

									filterDialog.ShowDialog();

									filter = filterDialog.filterString;
								}

								if (header == "WWEM")
								{
									command = "-extract_wwem_to_ogg_from";

									progress.message.Content = "Extracting All " + header + " To OGG To IOI Paths...";
								}
								else if (header == "WWES")
								{
									command = "-extract_wwes_to_ogg_from";

									progress.message.Content = "Extracting All " + header + " To OGG To IOI Paths...";
								}
								else if (header == "WWEV")
								{
									command = "-extract_wwev_to_ogg_from";

									progress.message.Content = "Extracting All " + header + " To OGG To IOI Paths...";
								}
								else if (header == "ORES")
								{
									command = "-extract_ores_from";

									progress.message.Content = "Extracting All " + header + " To IOI Paths...";
								}
								else if (header == "GFXI")
								{
									command = "-extract_ores_from";

									filter = header;

									progress.message.Content = "Extracting All " + header + " To IOI Paths...";
								}
								else if (header == "JSON")
								{
									command = "-extract_ores_from";

									filter = header;

									progress.message.Content = "Extracting All " + header + " To IOI Paths...";
								}
							}
							else
							{
								return;
							}
						}
						else
						{
							string[] buttons = { "Extract All " + header, "Cancel" };

							RightClickMenu rightClickMenu = new RightClickMenu(buttons);

							SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

							rightClickMenu.ShowDialog();

							//MessageBoxShow(rightClickMenu.buttonPressed);
							//MessageBoxShow(rpkgFilePath);

							if (rightClickMenu.buttonPressed == "button0")
							{
								command = "-extract_from_rpkg";

								filter = header;

								progress.message.Content = "Extracting " + header + "...";
							}
							else
							{
								return;
							}
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
					else
					{
						string[] headerSplit = header.Split(' ');

						string hashName = headerSplit[0];

						rpkgFilePath = GetRootTreeNode(e.Node).Text;

						//MessageBoxShow(rpkgFilePath);

						string command = "";
						string input_path = rpkgFilePath;
						string filter = "";
						string search = "";
						string search_type = "";
						string output_path = userSettings.OutputFolder;

						Progress progress = new Progress();

						progress.operation = (int)Progress.Operation.MASS_EXTRACT;

						string[] hashData = hashName.Split('.');

						string hashValue = hashData[0];
						string hashType = hashData[1];

						RightClickMenu rightClickMenu;

						int buttonCount = 0;

						if (hashType == "PRIM")
						{
							string[] buttons = { "Extract " + hashName, "Extract " + hashName + " MODEL To GLB/TGA File(s)", "Extract " + hashName + " To GLB File", "Cancel" };

							buttonCount = 4;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "TEMP")
						{
							string[] buttons = { "Extract " + hashName, "Extract to QN (QuickEntity) JSON", "Edit " + hashName + " in Brick/Entity Editor (Recursive)", "Edit " + hashName + " in Brick/Entity Editor (Non-Recursive)", "Extract PRIM Models linked to " + hashName + " To GLB/TGA File(s)", "Extract PRIMs linked to " + hashName + " To GLB File(s)", "Cancel" };

							buttonCount = 7;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "TEXT")
						{
							string[] buttons = { "Extract " + hashName, "Extract TEXTs(TEXDs) linked to " + hashName + " To TGA File(s)", "Cancel" };

							buttonCount = 3;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "WWEM" || hashType == "WWES" || hashType == "WWEV")
						{
							string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To OGG (IOI Path)", "Cancel" };

							buttonCount = 3;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "DLGE" || hashType == "LOCR" || hashType == "RTLV")
						{
							string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To JSON (IOI Path)", "Cancel" };

							buttonCount = 3;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "GFXI")
						{
							if (header.Contains("[ores:"))
							{
								string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To Image (IOI Path)", "Cancel" };

								buttonCount = 3;

								rightClickMenu = new RightClickMenu(buttons);
							}
							else
							{
								string[] buttons = { "Extract " + hashName, "Cancel" };

								buttonCount = 2;

								rightClickMenu = new RightClickMenu(buttons);
							}
						}
						else if (hashType == "JSON")
						{
							if (header.Contains("[ores:"))
							{
								string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To JSON (IOI Path)", "Cancel" };

								buttonCount = 3;

								rightClickMenu = new RightClickMenu(buttons);
							}
							else
							{
								string[] buttons = { "Extract " + hashName, "Cancel" };

								buttonCount = 2;

								rightClickMenu = new RightClickMenu(buttons);
							}
						}
						else if (hashType == "AIRG" || hashType == "CRMD" || hashType == "ATMD" || hashType == "CBLU" || hashType == "CPPT" || hashType == "DSWB" || hashType == "GIDX" || hashType == "VIDB" || hashType == "WSGB")
						{
							string[] buttons = { "Extract " + hashName, "Extract To Hitman 3 RT (ResourceTool) JSON", "Extract To Hitman 2 RT (ResourceTool) JSON", "Extract To Hitman 2016 RT (ResourceTool) JSON", "Cancel" };

							buttonCount = 5;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "ECPB")
						{
							string[] buttons = { "Extract " + hashName, "Extract To Hitman 3 RT (ResourceTool) JSON", "Extract To Hitman 2 RT (ResourceTool) JSON", "Cancel" };

							buttonCount = 4;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "MATI")
						{
							string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To MATI JSON", "Cancel" };

							buttonCount = 3;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "SDEF")
						{
							string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To SDEF JSON", "Cancel" };

							buttonCount = 3;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "MRTR")
						{
							string[] buttons = { "Extract " + hashName, "Extract " + hashName + " To MRTR JSON", "Cancel" };

							buttonCount = 3;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else if (hashType == "GFXF")
						{
							string[] buttons = { "Extract " + hashName, "Extract To Hitman 3 RT (ResourceTool) JSON", "Extract To Hitman 2 RT (ResourceTool) JSON", "Extract To Hitman 2016 RT (ResourceTool) JSON", "Extract GFXF Files From " + hashName, "Cancel" };

							buttonCount = 6;

							rightClickMenu = new RightClickMenu(buttons);
						}
						else
						{
							string[] buttons = { "Extract " + hashName, "Cancel" };

							buttonCount = 2;

							rightClickMenu = new RightClickMenu(buttons);
						}

						SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

						rightClickMenu.ShowDialog();

						if (rightClickMenu.buttonPressed == "button0")
						{
							progress.ProgressBar.IsIndeterminate = true;

							command = "-extract_from_rpkg";

							filter = hashValue;

							progress.message.Content = "Extracting " + hashName + "...";

							string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

							if (outputFolder == "")
							{
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

									return;
								}

								/*List<string> rpkgFiles = new List<string>();

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
								}*/

								ImportRPKGFileFolder(runtimeDirectory);

								command = "-extract_prim_model_from";

								progress.operation = (int)Progress.Operation.GENERAL;

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To GLB/TGA File(s)...";

								string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

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

								return;
							}
							else if (hashType == "TEXT")
							{
								command = "-extract_text_from";

								progress.operation = (int)Progress.Operation.GENERAL;

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To TGA File(s)...";

								string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

								if (temp_outputFolder == "")
								{
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

								return;
							}
							else if (hashType == "TEMP")
							{
								string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

								if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
								{
									MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform QN (QuickEntity) JSON extraction.");

									return;
								}

								ImportRPKGFileFolder(runtimeDirectory);

								command = "-extract_entity_to_qn";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " entity (TEMP/TBLU) to QN (QuickEntity) JSON...";

								var fileDialog = new Ookii.Dialogs.Wpf.VistaSaveFileDialog();

								fileDialog.Title = "Select file to save extracted QN (QuickEntity) JSON to:";

								fileDialog.Filter = "JSON file|*.json";

								string initialFolder = "";

								if (File.Exists(userSettings.InputFolder))
								{
									initialFolder = userSettings.InputFolder;
								}
								else
								{
									initialFolder = System.IO.Directory.GetCurrentDirectory();
								}

								fileDialog.InitialDirectory = initialFolder;

								fileDialog.FileName = hashName.Split('.')[0] + ".entity.json";

								var fileDialogResult = fileDialog.ShowDialog();

								if (fileDialogResult == true)
								{
									output_path = fileDialog.FileName;
								}
								else
								{
									return;
								}
							}
							else if (hashType == "WWEM")
							{
								command = "-extract_wwem_to_ogg_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To OGG To IOI Path...";
							}
							else if (hashType == "WWES")
							{
								command = "-extract_wwes_to_ogg_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To OGG To IOI Path...";
							}
							else if (hashType == "WWEV")
							{
								command = "-extract_wwev_to_ogg_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To OGG To IOI Path...";
							}
							else if (hashType == "DLGE")
							{
								command = "-extract_dlge_to_json_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To JSON...";
							}
							else if (hashType == "LOCR")
							{
								command = "-extract_locr_to_json_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To JSON...";
							}
							else if (hashType == "RTLV")
							{
								command = "-extract_rtlv_to_json_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To JSON...";
							}
							else if (hashType == "GFXI")
							{
								command = "-extract_ores_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To Image To IOI Path...";
							}
							else if (hashType == "JSON")
							{
								command = "-extract_ores_from";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To JSON To IOI Path...";
							}
							else if (hashType == "AIRG" || hashType == "CRMD" || hashType == "ATMD" || hashType == "CBLU" || hashType == "CPPT" || hashType == "DSWB" || hashType == "ECPB" || hashType == "GFXF" || hashType == "GIDX" || hashType == "VIDB" || hashType == "WSGB")
							{
								command = "-extract_to_rt_json";

								filter = hashValue;

								search = "HM3";

								progress.message.Content = "Extracting " + hashName + " To HM3 RT (ResourceTool) JSON...";
							}
							else if (hashType == "MATI")
							{
								command = "-extract_mati_to_json";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " MATI to MATI JSON...";
							}
							else if (hashType == "SDEF")
							{
								command = "-extract_sdef_to_json";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " SDEF to SDEF JSON...";
							}
							else if (hashType == "MRTR")
							{
								command = "-extract_mrtr_to_json";

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " MRTR to MRTR JSON...";
							}

							if (hashType != "TEMP")
							{
								string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

								if (outputFolder == "")
								{
									return;
								}

								output_path = outputFolder;
							}
						}
						else if (rightClickMenu.buttonPressed == "button2" && buttonCount >= 4)
						{
							if (hashType == "PRIM")
							{
								command = "-extract_prim_to_glb_from";

								progress.operation = (int)Progress.Operation.GENERAL;

								filter = hashValue;

								progress.message.Content = "Extracting " + hashName + " To GLB File...";
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

								rpkgFilePath = rpkgFileBackup;

								//MessageBoxShow(rpkgFilePath);

								int temp_file_version = get_temp_version(hashName, rpkgFilePath);

								while (temp_file_version < 2 || temp_file_version > 3)
								{
									MessageQuestion messageBox = new MessageQuestion();

									if (temp_file_version == 4)
									{
										messageBox.message.Content = "The automatic TEMP/TBLU version check found a TEMP entry count but was still unable to determine what version of Hitman (H2 or H3) these files are.\n\nPlease select the correct version of Hitman H2 or H3 below.";
									}
									else if (temp_file_version == 5)
									{
										messageBox.message.Content = "The automatic TEMP/TBLU version check could not find a TEMP entry count,\n\nmost likely because this TEMP file was made by a version of ResourceTool that didn't include the TEMP subEntities count value.\n\nTherefore the version of Hitman (H2 or H3) was not able to be determined.\n\nPlease select the correct version of Hitman H2 or H3 below.";
									}
									else
									{
										messageBox.message.Content = "The automatic TEMP/TBLU version check was unable to determine what version of Hitman (H2 or H3) these files are.\n\nPlease select the correct version of Hitman H2 or H3 below.";
									}

									messageBox.OKButton.Content = "Hitman 2";
									messageBox.CancelButton.Content = "Hitman 3";
									messageBox.ShowDialog();

									if (messageBox.buttonPressed == "OKButton")
									{
										temp_file_version = 2;
									}
									else if (messageBox.buttonPressed == "CancelButton")
									{
										temp_file_version = 3;
									}
								}

								int temp_return_value = clear_temp_tblu_data();

								//MessageBoxShow(hashName + ", " + rpkgFilePath);

								rpkgFilePath = rpkgFileBackup;

								temp_return_value = reset_task_status();

								execute_load_recursive_temps load_recursive_temps_execute = load_recursive_temps;

								IAsyncResult temp_ar = load_recursive_temps_execute.BeginInvoke(hashName, rpkgFilePath, (UInt32)temp_file_version, null, null);

								Progress temp_progress = new Progress();

								temp_progress.message.Content = "Analyzing Entity/Brick (TEMP/TBLU)...";

								temp_progress.operation = (int)Progress.Operation.TEMP_TBLU;

								temp_progress.ShowDialog();

								int temp_index = get_temp_index(hashName);

								//MessageBoxShow(temp_index.ToString());

								if (temp_progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
								{
									if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_DEPENDS)
									{
										MessageBoxShow("Error: " + hashName + " file has no TBLU hash depends.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_RPKG)
									{
										if (rpkgUpperName.Contains("PATCH"))
										{
											string rpkgBaseName = rpkgFile.Substring(0, rpkgUpperName.IndexOf("PATCH")) + ".rpkg";

											MessageBoxShow("Error: TBLU file linked to " + hashName + " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.\n\nTry importing " + rpkgBaseName + " and trying to edit " + hashName + " again.\n\nThis should be done before trying to launch the Brick/Entity Editor.");
										}
										else
										{
											MessageBoxShow("Error: TBLU file linked to " + hashName + " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.");
										}
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_TOO_MANY)
									{
										MessageBoxShow("Error: " + hashName + " file has too many TBLU hash depends.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_HEADER_NOT_FOUND)
									{
										MessageBoxShow("Error: " + hashName + " file is an empty TEMP file, missing it's resource type header/footer.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_ENTRY_COUNT_MISMATCH)
									{
										MessageBoxShow("Error: " + hashName + " file and TBLU file have mismatched entry/entity counts.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_VERSION_UNKNOWN)
									{
										MessageBoxShow("Error: " + hashName + " file's version is unknown.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TBLU_VERSION_UNKNOWN)
									{
										MessageBoxShow("Error: " + hashName + " file's TBLU file's version is unknown.");
									}

									temp_return_value = clear_temp_tblu_data();
								}
								else
								{
									if (entityBrickEditor == null)
									{
										entityBrickEditor = new EntityBrickEditor();
									}

									string initialFolder = "";

									if (File.Exists(userSettings.InputFolder))
									{
										initialFolder = userSettings.InputFolder;
									}
									else
									{
										initialFolder = System.IO.Directory.GetCurrentDirectory();
									}

									entityBrickEditor.inputFolder = initialFolder;


									if (File.Exists(userSettings.OutputFolder))
									{
										initialFolder = userSettings.OutputFolder;
									}
									else
									{
										initialFolder = System.IO.Directory.GetCurrentDirectory();
									}

									entityBrickEditor.outputFolder = initialFolder;

									entityBrickEditor.temps_index = (UInt32)temp_index;
									entityBrickEditor.temp_file_version = temp_file_version;
									entityBrickEditor.tempFileName = hashName;
									entityBrickEditor.rpkgFilePath = rpkgFilePath;
									entityBrickEditor.tempFileNameFull = header;

									string[] theme = userSettings.ColorTheme.Split('/');

									entityBrickEditor.currentThemeBrightness = theme[0];
									string color = theme[1];

									SetDiscordStatus("Brick Editor", hashName);

									entityBrickEditor.ShowDialog();

									if (LeftTabControl.SelectedIndex == 0)
									{
										SetDiscordStatus("Resource View", "");
									}
									else if (LeftTabControl.SelectedIndex == 2)
									{
										SetDiscordStatus("REPO View", "");
									}
									else if (LeftTabControl.SelectedIndex == 4)
									{
										SetDiscordStatus("Search View", "");
									}
									else if (LeftTabControl.SelectedIndex == 6)
									{
										SetDiscordStatus("Deep Search View", "");
									}

									//GC.Collect();
									GC.WaitForPendingFinalizers();
									//GC.Collect();
								}

								return;
							}
							else if (hashType == "AIRG" || hashType == "CRMD" || hashType == "ATMD" || hashType == "CBLU" || hashType == "CPPT" || hashType == "DSWB" || hashType == "ECPB" || hashType == "GFXF" || hashType == "GIDX" || hashType == "VIDB" || hashType == "WSGB")
							{
								command = "-extract_to_rt_json";

								filter = hashValue;

								search = "HM2";

								progress.message.Content = "Extracting " + hashName + " To HM2 RT (ResourceTool) JSON...";
							}

							string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

							if (outputFolder == "")
							{
								return;
							}

							output_path = outputFolder;
						}
						else if (rightClickMenu.buttonPressed == "button3" && buttonCount >= 5)
						{
							if (hashType == "TEMP")
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

								rpkgFilePath = rpkgFileBackup;

								//MessageBoxShow(rpkgFilePath);

								int temp_file_version = get_temp_version(hashName, rpkgFilePath);

								while (temp_file_version < 2 || temp_file_version > 3)
								{
									MessageQuestion messageBox = new MessageQuestion();

									if (temp_file_version == 4)
									{
										messageBox.message.Content = "The automatic TEMP/TBLU version check found a TEMP entry count but was still unable to determine what version of Hitman (H2 or H3) these files are.\n\nPlease select the correct version of Hitman H2 or H3 below.";
									}
									else if (temp_file_version == 5)
									{
										messageBox.message.Content = "The automatic TEMP/TBLU version check could not find a TEMP entry count,\n\nmost likely because this TEMP file was made by a version of ResourceTool that didn't include the TEMP subEntities count value.\n\nTherefore the version of Hitman (H2 or H3) was not able to be determined.\n\nPlease select the correct version of Hitman H2 or H3 below.";
									}
									else
									{
										messageBox.message.Content = "The automatic TEMP/TBLU version check was unable to determine what version of Hitman (H2 or H3) these files are.\n\nPlease select the correct version of Hitman H2 or H3 below.";
									}

									messageBox.OKButton.Content = "Hitman 2";
									messageBox.CancelButton.Content = "Hitman 3";
									messageBox.ShowDialog();

									if (messageBox.buttonPressed == "OKButton")
									{
										temp_file_version = 2;
									}
									else if (messageBox.buttonPressed == "CancelButton")
									{
										temp_file_version = 3;
									}
								}

								int temp_return_value = clear_temp_tblu_data();

								//MessageBoxShow(hashName + ", " + rpkgFilePath);

								rpkgFilePath = rpkgFileBackup;

								temp_return_value = reset_task_status();

								execute_load_recursive_temps load_recursive_temps_execute = load_non_recursive_temps;

								IAsyncResult temp_ar = load_recursive_temps_execute.BeginInvoke(hashName, rpkgFilePath, (UInt32)temp_file_version, null, null);

								Progress temp_progress = new Progress();

								temp_progress.message.Content = "Analyzing Entity/Brick (TEMP/TBLU)...";

								temp_progress.operation = (int)Progress.Operation.TEMP_TBLU;

								temp_progress.ShowDialog();

								int temp_index = get_temp_index(hashName);

								//MessageBoxShow(temp_index.ToString());

								if (temp_progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
								{
									if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_DEPENDS)
									{
										MessageBoxShow("Error: " + hashName + " file has no TBLU hash depends.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_RPKG)
									{
										if (rpkgUpperName.Contains("PATCH"))
										{
											string rpkgBaseName = rpkgFile.Substring(0, rpkgUpperName.IndexOf("PATCH")) + ".rpkg";

											MessageBoxShow("Error: TBLU file linked to " + hashName + " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.\n\nTry importing " + rpkgBaseName + " and trying to edit " + hashName + " again.\n\nThis should be done before trying to launch the Brick/Entity Editor.");
										}
										else
										{
											MessageBoxShow("Error: TBLU file linked to " + hashName + " file is missing.\n\nMake sure you you import the base archives if you are trying to edit a TEMP file residing in a patch RPKG.");
										}
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_TOO_MANY)
									{
										MessageBoxShow("Error: " + hashName + " file has too many TBLU hash depends.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_HEADER_NOT_FOUND)
									{
										MessageBoxShow("Error: " + hashName + " file is an empty TEMP file, missing it's resource type header/footer.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_TBLU_ENTRY_COUNT_MISMATCH)
									{
										MessageBoxShow("Error: " + hashName + " file and TBLU file have mismatched entry/entity counts.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TEMP_VERSION_UNKNOWN)
									{
										MessageBoxShow("Error: " + hashName + " file's version is unknown.");
									}
									else if (temp_progress.task_status == (int)Progress.RPKGStatus.TBLU_VERSION_UNKNOWN)
									{
										MessageBoxShow("Error: " + hashName + " file's TBLU file's version is unknown.");
									}

									temp_return_value = clear_temp_tblu_data();
								}
								else
								{
									if (entityBrickEditor == null)
									{
										entityBrickEditor = new EntityBrickEditor();
									}

									string initialFolder = "";

									if (File.Exists(userSettings.InputFolder))
									{
										initialFolder = userSettings.InputFolder;
									}
									else
									{
										initialFolder = System.IO.Directory.GetCurrentDirectory();
									}

									entityBrickEditor.inputFolder = initialFolder;


									if (File.Exists(userSettings.OutputFolder))
									{
										initialFolder = userSettings.OutputFolder;
									}
									else
									{
										initialFolder = System.IO.Directory.GetCurrentDirectory();
									}

									entityBrickEditor.outputFolder = initialFolder;

									entityBrickEditor.temps_index = (UInt32)temp_index;
									entityBrickEditor.temp_file_version = temp_file_version;
									entityBrickEditor.tempFileName = hashName;
									entityBrickEditor.rpkgFilePath = rpkgFilePath;
									entityBrickEditor.tempFileNameFull = header;

									string[] theme = userSettings.ColorTheme.Split('/');

									entityBrickEditor.currentThemeBrightness = theme[0];
									string color = theme[1];

									SetDiscordStatus("Brick Editor", hashName);

									entityBrickEditor.ShowDialog();

									if (LeftTabControl.SelectedIndex == 0)
									{
										SetDiscordStatus("Resource View", "");
									}
									else if (LeftTabControl.SelectedIndex == 2)
									{
										SetDiscordStatus("REPO View", "");
									}
									else if (LeftTabControl.SelectedIndex == 4)
									{
										SetDiscordStatus("Search View", "");
									}
									else if (LeftTabControl.SelectedIndex == 6)
									{
										SetDiscordStatus("Deep Search View", "");
									}

									//GC.Collect();
									GC.WaitForPendingFinalizers();
									//GC.Collect();
								}

								return;
							}
							else if (hashType == "AIRG" || hashType == "CRMD" || hashType == "ATMD" || hashType == "CBLU" || hashType == "CPPT" || hashType == "DSWB" || hashType == "GFXF" || hashType == "GIDX" || hashType == "VIDB" || hashType == "WSGB")
							{
								command = "-extract_to_rt_json";

								filter = hashValue;

								search = "HM2016";

								progress.message.Content = "Extracting " + hashName + " To HM2016 RT (ResourceTool) JSON...";
							}

							string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

							if (outputFolder == "")
							{
								return;
							}

							output_path = outputFolder;
						}
						else if (rightClickMenu.buttonPressed == "button4" && buttonCount >= 6)
						{
							if (hashType == "TEMP")
							{
								string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

								if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
								{
									MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform PRIM model extraction.");

									return;
								}

								/*List<string> rpkgFiles = new List<string>();

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
								}*/

								ImportRPKGFileFolder(runtimeDirectory);

								command = "-extract_all_prim_model_of_temp_from";

								filter = hashValue;

								progress.message.Content = "Extracting PRIM Models linked to " + hashName + " To GLB/TGA File(s)...";

								string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", ""); ;

								if (temp_outputFolder == "")
								{
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

								return;
							}
							else if (hashType == "GFXF")
							{
								command = "-extract_gfxf_from";

								filter = hashValue;

								progress.message.Content = "Extracting GFXF Files From" + hashName + "...";
							}

							if (hashType != "TEMP")
							{
								string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

								if (outputFolder == "")
								{
									return;
								}

								output_path = outputFolder;
							}
						}
						else if (rightClickMenu.buttonPressed == "button5" && buttonCount == 7)
						{
							if (hashType == "TEMP")
							{
								command = "-extract_all_prim_of_temp_from";

								filter = hashValue;

								progress.message.Content = "Extracting PRIMs linked to " + hashName + " To GLB File(s)...";
							}

							string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + hashName + " To:", "");

							if (outputFolder == "")
							{
								return;
							}

							output_path = outputFolder;
						}
						else
						{
							return;
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
			}
		}

		private void SearchHashListTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			if (oneOrMoreRPKGsHaveBeenImported)
			{
				if (MainTreeView.Nodes.Count > 0)
				{
					System.Windows.Forms.TreeNode item = (e.Node as System.Windows.Forms.TreeNode);

					if (item == null)
					{
						return;
					}

					string itemHeader = item.Text;

					//MessageBoxShow(itemHeader);

					string[] header = itemHeader.Split(' ');

					string hashFile = header[0];

					string[] hashDetails = hashFile.Split('.');

					string hash = hashDetails[0];
					string resourceType = hashDetails[1];

					string rpkgFilePath = GetRootTreeNode(e.Node).Text;

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

							string currentRPKGFilePath = rpkgHashFilePath;

							currentHash = header[0];

							if (RightTabControl.SelectedIndex == 2)
							{
								LoadHexEditor();
							}

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
										ThirdTabRightSeparator.Visibility = Visibility.Visible;
									}
								}
								else
								{
									if (ThirdTabRight.Visibility == Visibility.Visible)
									{
										ThirdTabRight.Visibility = Visibility.Collapsed;
										ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
									}
								}
							}
							else
							{
								if (ThirdTabRight.Visibility == Visibility.Visible)
								{
									ThirdTabRight.Visibility = Visibility.Collapsed;
									ThirdTabRightSeparator.Visibility = Visibility.Collapsed;
								}

								if (FourthTabRight.Visibility == Visibility.Visible)
								{
									FourthTabRight.Visibility = Visibility.Collapsed;
									FourthTabRightSeparator.Visibility = Visibility.Collapsed;
								}

								if (FifthTabRight.Visibility == Visibility.Visible)
								{
									FifthTabRight.Visibility = Visibility.Collapsed;
									FifthTabRightSeparator.Visibility = Visibility.Collapsed;
								}

								if (SixthTabRight.Visibility == Visibility.Visible)
								{
									SixthTabRight.Visibility = Visibility.Collapsed;
									SixthTabRightSeparator.Visibility = Visibility.Collapsed;
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

					//GC.Collect();
				}
			}
			else
			{
				DetailsTextBox.Text = "Hash is not located in any of the currently imported RPKGs.";

				HexViewerTextBox.Text = "";

				LocalizationTextBox.Text = "";
			}
		}

		private void SearchTextChanged()
		{
			if (searchRPKGsInputTimer == null)
			{
				searchRPKGsInputTimer = new System.Windows.Threading.DispatcherTimer();

				searchRPKGsInputTimer.Interval = TimeSpan.FromMilliseconds(600);

				searchRPKGsInputTimer.Tick += SearchRPKGsTextBox_TimerTimeout;
			}

			searchRPKGsInputTimer.Stop();
			//searchRPKGsInputTimer.Tag = (sender as TextBox).Text;
			searchRPKGsInputTimer.Start();
		}

		private void SearchRPKGsTextBox_TextChanged(object sender, TextChangedEventArgs e)
		{
			SearchTextChanged();
		}

		private void SearchRPKGsType_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			SearchTextChanged();
		}

		private void SearchRPKGsComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			SearchTextChanged();
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
					string rpkgPath = mainTreeNode.Text.ToString();

					bool rpkgItemAdded = false;

					//MessageBoxShow(rpkgPath);

					int rpkgResultsCount = 0;

					int resourceTypeItemIndex = 0;

					string resourceTypeFilter = "";
					bool resourceTypeFilterEnabled = false;

					if (SearchRPKGsType.Text != "All")
					{
						resourceTypeFilter = SearchRPKGsType.Text;
						resourceTypeFilterEnabled = true;
					}

					foreach (System.Windows.Forms.TreeNode resourceItems in mainTreeNode.Nodes)
					{
						string resourceType = resourceItems.Text.ToString();

						if (resourceTypeFilter == resourceType || !resourceTypeFilterEnabled)
						{
							if (rpkgResultsCount <= maxSearchResults)
							{
								bool resourceTypeItemAdded = false;

								//MessageBoxShow(resourceType);

								int return_value = search_imported_hashes(SearchRPKGsTextBox.Text, rpkgPath, resourceType, maxSearchResults);

								//MessageBoxShow(Marshal.PtrToStringAnsi(get_search_imported_hashes()));

								string searchResultsString = Marshal.PtrToStringAnsi(get_search_imported_hashes());

								bool found = false;

								System.Windows.Forms.TreeNode item;

								if (!rpkgItemAdded)
								{
									item = new System.Windows.Forms.TreeNode();
								}
								else
								{
									item = (SearchRPKGsTreeView.Nodes[rpkgItemIndex] as System.Windows.Forms.TreeNode);
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
											item2 = (item.Nodes[resourceTypeItemIndex] as System.Windows.Forms.TreeNode);
										}

										foreach (string searchResult in searchResults)
										{
											found = true;

											rpkgResultsCount++;

											//MessageBoxShow(searchResult);

											//var item3 = new System.Windows.Forms.TreeNode();

											string[] temp_test = searchResult.Split('.');

											string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

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

				int return_value = search_hash_list(SearchHashListTextBox.Text, maxSearchResults);

				string searchResultsString = Marshal.PtrToStringAnsi(get_search_hash_list());

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

							string ioiString = Marshal.PtrToStringAnsi(get_hash_list_string(temp_test[0]));

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

			//MessageBoxShow(tab.SelectedIndex.ToString());

			if (tab.SelectedIndex == 0)
			{
				SetDiscordStatus("Resource View", "");

				if (RightTabControl.Visibility == Visibility.Collapsed)
				{
					RightTabControl.Visibility = Visibility.Visible;
				}

				if (REPOGrid.Visibility == Visibility.Visible)
				{
					REPOGrid.Visibility = Visibility.Collapsed;
				}
			}
			else if (tab.SelectedIndex == 2)
			{
				SetDiscordStatus("REPO View", "");

				if (RightTabControl.Visibility == Visibility.Visible)
				{
					RightTabControl.Visibility = Visibility.Collapsed;
				}

				if (REPOGrid.Visibility == Visibility.Collapsed)
				{
					REPOGrid.Visibility = Visibility.Visible;
				}

				//if (REPOTreeView.Nodes.Count > 0)
				//{
					//if (REPOTreeView.Nodes[0].Text.StartsWith("NOTE:"))
					//{
						//LoadREPO();
					//}
				//}
			}
			else if (tab.SelectedIndex == 4)
			{
				SetDiscordStatus("Search View", "");

				if (RightTabControl.Visibility == Visibility.Collapsed)
				{
					RightTabControl.Visibility = Visibility.Visible;
				}

				if (REPOGrid.Visibility == Visibility.Visible)
				{
					REPOGrid.Visibility = Visibility.Collapsed;
				}
			}
			else if (tab.SelectedIndex == 6)
			{
				SetDiscordStatus("Deep Search View", "");

				if (RightTabControl.Visibility == Visibility.Collapsed)
				{
					RightTabControl.Visibility = Visibility.Visible;
				}

				if (REPOGrid.Visibility == Visibility.Visible)
				{
					REPOGrid.Visibility = Visibility.Collapsed;
				}
			}
		}

		private void RightTabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			TabControl tab = (sender as TabControl);

			//MessageBoxShow(tab.SelectedIndex.ToString());

			if (tab.SelectedIndex == 2)
			{
				LoadHexEditor();
			}

			if (tab.SelectedIndex == 6)
			{
				string[] hashDetails = currentHash.Split('.');
				string hash = hashDetails[0];

				if (hashDetails[1] == "TEXT")
				{
					LoadImageViewer_TEXT(currentHash, hash);
				}
				else if (hashDetails[1] == "GFXI")
				{
					LoadImageViewer_GFXI(hash);
				}
			}

			if (tab.SelectedIndex == 10)
			{
				string[] hashDetails = currentHash.Split('.');
				string hash = hashDetails[0];

				if (hashDetails[1] == "PRIM")
				{
					LoadModelViewer(hash);
				}
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
			if (currentHash != "")
			{
				HexViewerTextBox.Text = "Hex view of " + currentHash + ":\n\n";

				string[] hashDetails = currentHash.Split('.');

				string hash = hashDetails[0];

				HexViewerTextBox.AppendText(Marshal.PtrToStringAnsi(get_hash_in_rpkg_data_in_hex_view(rpkgFilePath, hash)));
			}
		}

		private void LoadImageViewer_TEXT(string hashFile, string hash)
		{
			try
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
						FourthTabRightSeparator.Visibility = Visibility.Visible;
					}

					File.Delete(png_file_name);
				}
				else
				{
					if (FourthTabRight.Visibility == Visibility.Visible)
					{
						FourthTabRight.Visibility = Visibility.Collapsed;
						FourthTabRightSeparator.Visibility = Visibility.Collapsed;
						RightTabControl.SelectedIndex = 0;
					}
				}

				int return_value_clear = clear_hash_data_vector();
			}
			catch
			{
				return;
			}
		}

		private void LoadImageViewer_GFXI(string hash)
		{
			try
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
						FourthTabRightSeparator.Visibility = Visibility.Visible;
					}
				}
				else
				{
					if (FourthTabRight.Visibility == Visibility.Visible)
					{
						FourthTabRight.Visibility = Visibility.Collapsed;
						FourthTabRightSeparator.Visibility = Visibility.Collapsed;
						RightTabControl.SelectedIndex = 0;
					}
				}

				int return_value_clear = clear_hash_data_vector();
			}
			catch
			{
				return;
			}
		}

		private void LoadModelViewer(string hash)
		{
			string command = "-extract_prim_to_obj_from";
			string input_path = rpkgFilePath;
			string filter = hash;
			string search = "";
			string search_type = "";
			string output_path = "";

			int return_value = reset_task_status();

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
					SixthTabRightSeparator.Visibility = Visibility.Visible;
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
					SixthTabRightSeparator.Visibility = Visibility.Collapsed;
					RightTabControl.SelectedIndex = 0;
				}
			}
		}

		public static void TEXTToPNGThread(string rpkgFilePathLocal, string hash, string png_file_name)
		{
			int return_value = generate_png_from_text(rpkgFilePathLocal, hash, png_file_name);
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

			foreach (System.Windows.Forms.TreeNode treeViewNode in MainTreeView.Nodes)
			{
				if (rpkgFilePath == (treeViewNode.Text as string))
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

			if (MainTreeView.Nodes.Count > 0)
			{
				if ((MainTreeView.Nodes[0] as System.Windows.Forms.TreeNode).Text.ToString() == "Click")
				{
					MainTreeView.Nodes.Clear();
				}
			}

			MainTreeView.AfterExpand += MainTreeView_AfterExpand;

			var item = new System.Windows.Forms.TreeNode();

			item.Text = rpkgFilePath;

			MainTreeView.Nodes.Add(item);

			resetDeepSearchComboBoxes();

			//LoadREPO();

			List<string> resourceTypes = new List<string>();

			UInt32 resourceTypeCount = get_resource_types_count(rpkgFilePath);

			for (UInt32 i = 0; i < resourceTypeCount; i++)
			{
				resourceTypes.Add(Marshal.PtrToStringAnsi(get_resource_types_at(rpkgFilePath, i)));
			}

			resourceTypes.Sort();

			foreach (string resourceType in resourceTypes)
			{
				var item2 = new System.Windows.Forms.TreeNode();

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
				else if (LeftTabControl.SelectedIndex == 2)
				{
					SetDiscordStatus("REPO View", "");
				}
				else if (LeftTabControl.SelectedIndex == 4)
				{
					SetDiscordStatus("Search View", "");
				}
				else if (LeftTabControl.SelectedIndex == 6)
				{
					SetDiscordStatus("Deep Search View", "");
				}
			}

			oneOrMoreRPKGsHaveBeenImported = true;
		}

		private void ImportRPKGFileFolder(string folderPath)
		{
			List<string> rpkgFiles = new List<string>();

			foreach (var filePath in Directory.GetFiles(folderPath))
			{
				if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
				{
					rpkgFiles.Add(filePath);
				}
			}

			rpkgFiles.Sort(new NaturalStringComparer());

			string rpkgListString = "";

			List<string> rpkgList = new List<string>();

			foreach (string filePath in rpkgFiles)
			{
				bool found = false;

				foreach (System.Windows.Forms.TreeNode treeViewNode in MainTreeView.Nodes)
				{
					if (filePath == (treeViewNode.Text as string))
					{
						found = true;
					}
				}

				if (!found)
				{
					rpkgListString += filePath + ",";

					rpkgList.Add(filePath);
				}
			}

			int return_value = reset_task_status();

			execute_import_rpkgs temp_rpkgExecute = import_rpkgs;

			IAsyncResult temp_ar = temp_rpkgExecute.BeginInvoke(folderPath, rpkgListString, null, null);

			Progress progress = new Progress();

			progress.message.Content = "Importing RPKG file(s) from " + folderPath + "...";

			progress.operation = (int)Progress.Operation.MASS_EXTRACT;

			progress.ShowDialog();

			if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
			{
				//MessageBoxShow(progress.task_status_string);

				return;
			}

			foreach (string filePath in rpkgList)
			{
				int rpkg_valid = is_rpkg_valid(filePath);

				if (rpkg_valid == 1)
				{
					if (MainTreeView.Nodes.Count > 0)
					{
						if ((MainTreeView.Nodes[0] as System.Windows.Forms.TreeNode).Text.ToString() == "Click")
						{
							MainTreeView.Nodes.Clear();
						}
					}

					MainTreeView.AfterExpand += MainTreeView_AfterExpand;

					var item = new System.Windows.Forms.TreeNode();

					item.Text = filePath;

					MainTreeView.Nodes.Add(item);

					List<string> resourceTypes = new List<string>();

					UInt32 resourceTypeCount = get_resource_types_count(filePath);

					for (UInt32 i = 0; i < resourceTypeCount; i++)
					{
						resourceTypes.Add(Marshal.PtrToStringAnsi(get_resource_types_at(filePath, i)));
					}

					resourceTypes.Sort();

					foreach (string resourceType in resourceTypes)
					{
						var item2 = new System.Windows.Forms.TreeNode();

						item2.Text = resourceType;

						item2.Nodes.Add("");

						//item2.Collapsed += Item2_Collapsed;

						item.Nodes.Add(item2);
					}
				}
			}

			resetDeepSearchComboBoxes();

			//LoadREPO();

			if (oneOrMoreRPKGsHaveBeenImported)
			{
				if (LeftTabControl.SelectedIndex == 0)
				{
					SetDiscordStatus("Resource View", "");
				}
				else if (LeftTabControl.SelectedIndex == 2)
				{
					SetDiscordStatus("REPO View", "");
				}
				else if (LeftTabControl.SelectedIndex == 4)
				{
					SetDiscordStatus("Search View", "");
				}
				else if (LeftTabControl.SelectedIndex == 6)
				{
					SetDiscordStatus("Deep Search View", "");
				}
			}

			oneOrMoreRPKGsHaveBeenImported = true;

			runtimeDirLoaded = true;
		}

		private void OpenRPKGFile_Click(object sender, RoutedEventArgs e)
		{
			string rpkgPath = SelectFile("input", "Select an RPKG file to import:", "RPKG file|*.rpkg|All files|*.*", "");

			if (rpkgPath != "")
			{
				ImportRPKGFile(rpkgPath);

				//LoadHashDependsMap();
			}
		}

		private void Extract_Click(object sender, RoutedEventArgs e)
		{
			var fileDialog = new Ookii.Dialogs.Wpf.VistaOpenFileDialog();

			fileDialog.Title = "Select an RPKG file to extract from:";

			fileDialog.Filter = "RPKG file|*.rpkg|All files|*.*";

			string initialFolder = "";

			if (File.Exists(userSettings.InputFolder))
			{
				initialFolder = userSettings.InputFolder;
			}
			else
			{
				initialFolder = System.IO.Directory.GetCurrentDirectory();
			}

			fileDialog.InitialDirectory = initialFolder;

			fileDialog.FileName = initialFolder;

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

			string outputFolder = SelectFolder("output", "Select Output Folder To Extract RPKG To:", "");

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

			string inputFolder = "";

			inputFolder = SelectFolder("input", "Select Input Folder (Runtime folder or other folder with multiple RPKGs) To Extract " + massExtractName + " From:", "");

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

			ImportRPKGFileFolder(inputFolder);

			string outputFolder = SelectFolder("output", "Select Output Folder To Extract " + massExtractName + " To:", "");

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

			string inputFolder = SelectFolder("input", "Select Folder To Rebuild " + rebuildType + " From:", "");

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
			string inputFolder = SelectFolder("input", "Select Folder Containing RPKG Files To Import From:", "");

			if (inputFolder != "")
			{
				ImportRPKGFileFolder(inputFolder);

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
			string inputFolder = SelectFolder("input", "Select Folder To Generate RPKG From:", "");

			if (inputFolder != "")
			{
				string outputFolder = SelectFolder("output", "Select Output Folder To Save Generated RPKG To:", "");

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

		private string SelectFolder(string type, string description, string initialFolder)
		{
			var folderDialog = new Ookii.Dialogs.Wpf.VistaFolderBrowserDialog();

			folderDialog.Description = description;

			folderDialog.UseDescriptionForTitle = true;

			if (type == "input")
			{
				if (!Directory.Exists(initialFolder))
				{
					if (Directory.Exists(userSettings.InputFolder))
					{
						initialFolder = userSettings.InputFolder;
					}
					else
					{
						initialFolder = Directory.GetCurrentDirectory();
					}
				}
			}
			else if (type == "output")
			{
				if (!Directory.Exists(initialFolder))
				{
					if (Directory.Exists(userSettings.OutputFolder))
					{
						initialFolder = userSettings.OutputFolder;
					}
					else
					{
						initialFolder = Directory.GetCurrentDirectory();
					}
				}
			}
			folderDialog.SelectedPath = initialFolder + Path.DirectorySeparatorChar;

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

		private string SelectFile(string type, string description, string filter, string initialFileName)
		{
			if (type == "input")
			{
				var fileDialog = new Ookii.Dialogs.Wpf.VistaOpenFileDialog();

				fileDialog.Title = description;

				fileDialog.Filter = filter;

				string initialFolder = "";

				if (File.Exists(userSettings.InputFolder))
				{
					initialFolder = userSettings.InputFolder;
				}
				else
				{
					initialFolder = System.IO.Directory.GetCurrentDirectory();
				}

				fileDialog.InitialDirectory = initialFolder;

				var fileDialogResult = fileDialog.ShowDialog();

				if (fileDialogResult == true)
				{
					return fileDialog.FileName;
				}
			}
			else if (type == "output")
			{
				var fileDialog = new Ookii.Dialogs.Wpf.VistaSaveFileDialog();

				fileDialog.Title = description;

				fileDialog.Filter = filter;

				string initialFolder = "";

				if (File.Exists(userSettings.InputFolder))
				{
					initialFolder = userSettings.InputFolder;
				}
				else
				{
					initialFolder = System.IO.Directory.GetCurrentDirectory();
				}

				fileDialog.InitialDirectory = initialFolder;

				fileDialog.FileName = initialFileName;

				var fileDialogResult = fileDialog.ShowDialog();

				if (fileDialogResult == true)
				{
					return fileDialog.FileName;
				}
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

			if (brightness == "Dark")
			{
				MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
				SearchRPKGsTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				SearchRPKGsTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
				DeepSearchEntitiesTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				DeepSearchEntitiesTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
				DeepSearchLocalizationTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				DeepSearchLocalizationTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
				REPOTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				REPOTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
				SearchHashListTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				SearchHashListTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#252525");
				xshdJSONDark = XmlReader.Create(new MemoryStream(Properties.Resources.JSON_Dark));
				REPOJSONTextEditor.SyntaxHighlighting = HighlightingLoader.Load(xshdJSONDark, HighlightingManager.Instance);
				REPOJSONTextEditor.Foreground = System.Windows.Media.Brushes.White;
				HexViewerTextBox.Foreground = System.Windows.Media.Brushes.White;
			}
			else if (brightness == "Light")
			{
				MainTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
				MainTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				SearchRPKGsTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
				SearchRPKGsTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				DeepSearchEntitiesTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
				DeepSearchEntitiesTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				DeepSearchLocalizationTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
				DeepSearchLocalizationTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				REPOTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
				REPOTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				SearchHashListTreeView.ForeColor = System.Drawing.ColorTranslator.FromHtml("#000000");
				SearchHashListTreeView.BackColor = System.Drawing.ColorTranslator.FromHtml("#FFFFFF");
				xshdJSONLight = XmlReader.Create(new MemoryStream(Properties.Resources.JSON_Light));
				REPOJSONTextEditor.SyntaxHighlighting = HighlightingLoader.Load(xshdJSONLight, HighlightingManager.Instance);
				REPOJSONTextEditor.Foreground = System.Windows.Media.Brushes.Black;
				HexViewerTextBox.Foreground = System.Windows.Media.Brushes.Black;
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

		private UserSettings userSettings;
		private bool oneOrMoreRPKGsHaveBeenImported = false;
		private string rpkgFilePath = "";
		private string currentHash = "";
		private string currentHashFileName = "";
		public string hashDependsRPKGFilePath = "";
		private string currentNodeText = "";
		private System.Windows.Threading.DispatcherTimer searchREPOInputTimer;
		private System.Windows.Threading.DispatcherTimer searchRPKGsInputTimer;
		private System.Windows.Threading.DispatcherTimer searchHashListInputTimer;
		private System.Windows.Threading.DispatcherTimer OGGPlayerTimer;
		private NAudio.Wave.WaveOut waveOut;
		private MemoryStream pcmMemoryStream;
		private int pcmSampleSize;
		private int pcmSampleRate;
		private int pcmChannels;
		public EntityBrickEditor entityBrickEditor;
		public List<string> patchHashOriginalList;
		public List<string> patchHashList;
		public List<string> patchHashNamesList;
		public List<string> hashDependsOriginalList;
		public List<string> hashDependsList;
		public List<string> hashDependsNamesList;
		public List<string> hashDependsFlagsList;
		public int hashDependsPage;
		public bool discordOn = false;
		public DiscordRPC.Timestamps timestamp;
		public bool runtimeDirLoaded = false;
		BackgroundWorker deepSearchEntitiesWorker;
		BackgroundWorker deepSearchLocalizationWorker;
		XmlReader xshdJSONDark;
		XmlReader xshdJSONLight;
		public TreeViewBackup repoTreeViewBackup;
		public string repoJSONTextEditorID;
		public int repoJSONTextEditorParentNodeIndex;
		public int repoJSONTextEditorNodeIndex;
		Dictionary<string, string[]> enumDictionary;
		JsonDocument visualEditorJSON;
		bool loadingVisualEditor = false;

private enum OggPlayerState
		{
			NULL,
			READY,
			PLAYING,
			PAUSED,
			RESET
		}

		public enum Operation
		{
			IMPORT,
			GENERAL,
			MASS_EXTRACT,
			DOWNLOAD,
			TEMP_TBLU,
			PRIM,
			PRIM_REBUILD,
			PRIM_MODEL_REBUILD,
			PRIM_MODEL_EXTRACT,
			DEEP_SEARCH
		};

		public enum RPKGStatus
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
			TBLU_VERSION_UNKNOWN,
			PRIM_REBUILD_GLB_MESH_NAME_MALFORMED,
			PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED,
			PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3,
			PRIM_REBUILD_MISSING_POSITION_DATA,
			PRIM_REBUILD_MISMATCHED_BONES,
			PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM,
			PRIM_REBUILD_WEIGHTED_DATA_MISSING,
			PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES,
			PRIM_REBUILD_MISSING_NORMAL_DATA,
			PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES,
			PRIM_REBUILD_MISSING_UV_DATA,
			PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES,
			PRIM_REBUILD_COLORS_WRONG_FORMAT,
			PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS,
			PRIM_REBUILD_META_FILE_MISSING,
			PRIM_REBUILD_SUCCESSFUL,
			PRIM_MODEL_REBUILD_SUCCESSFUL,
			PRIM_MODEL_EXTRACT_SUCCESSFUL,
			MAP_RECURSIVE_TEMP_LOADING_EXECUTING,
			MAP_GET_MAP_NODES_EXECUTING,
			MAP_EXTRACT_MAP_NODES_PRIMS_EXECUTING,
			MAP_GENERATE_GODOT_PROJECT_EXECUTING,
			MAP_EXPORT_SUCCESSFUL,
			MAP_GODOT_MAP_NODE_IMPORTING,
			MAP_NODE_TEMP_LOADING,
			MAP_NODE_CHANGES_CHECK,
			MAP_WRITING_CHANGES_TO_QN,
			MAP_IMPORT_SUCCESSFUL,
			MAP_ERROR
		};

		public enum REPOCategory
		{
			NPCS,
			OUTFITS,
			RUNTIME_QUESTITEMS,
			RUNTIME_MELEES,
			RUNTIME_KEYS,
			RUNTIME_EXPLOSIVES,
			RUNTIME_TOOLS,
			RUNTIME_PISTOLS,
			RUNTIME_INVALID_CATEGORY_ICONS,
			RUNTIME_SNIPERRIFLES,
			RUNTIME_ASSAULTRIFLES,
			RUNTIME_REMOTES,
			RUNTIME_SHOTGUNS,
			RUNTIME_SUITCASES,
			RUNTIME_SMGS,
			RUNTIME_DISTRACTIONS,
			RUNTIME_POISONS,
			RUNTIME_CONTAINERS,
			AREADISCOVEREDS,
			LOCATIONS,
			MODIFIERS,
			MAGAZINE_CONFIGS,
			AMMO_CONFIGS,
			DIFFICULTY_PARAMETERS,
			BEHAVIOURS
		};

		private int oggPlayerState = (int)OggPlayerState.NULL;
		private bool oggPlayerRunning = false;
		private bool oggPlayerPaused = false;
		private bool oggPlayerStopped = false;
		private bool oggPlayerStoppedNew = false;
		FileInfo file;
		FileStream stream;
		WaveFormat waveFormat;
		NAudio.Wave.RawSourceWaveStream pcmSource;

		public delegate int execute_import_rpkgs(string rpkgs_path, string rpkgs_list);
		public delegate int execute_load_recursive_temps(string temp_hash, string rpkg_file_path, UInt32 temp_version);
		public delegate int execute_get_hashes_with_no_reverse_depends(string rpkg_file);
		public delegate int execute_get_direct_hash_depends(string rpkg_file, string hash_string);
		public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter, string search, string search_type, string csharp_output_path);
		public delegate int execute_deep_search_localization(string input_path, string search_value, int search_dlge, int search_locr, int search_rtlv, int max_results);
		public delegate int execute_deep_search_entities(string input_path, string search_value, int search_entity_ids, int search_entity_names, int search_property_names, int search_property_values, int max_results);

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

		[DllImport("rpkg.dll", EntryPoint = "generate_localization_line_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 generate_localization_line_string(string rpkg_file_name, string hash_string, string resource_type);

		[DllImport("rpkg.dll", EntryPoint = "get_localization_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_localization_string();

		[DllImport("rpkg.dll", EntryPoint = "get_localization_line_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_localization_line_string();

		[DllImport("rpkg.dll", EntryPoint = "generate_json_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 generate_json_string(string rpkg_file_name, string hash_string);

		[DllImport("rpkg.dll", EntryPoint = "get_json_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_json_string();

		[DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_hashes_with_no_reverse_depends();

		[DllImport("rpkg.dll", EntryPoint = "load_hash_list", CallingConvention = CallingConvention.Cdecl)]
		public static extern int load_hash_list(string path);

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
		public static extern int load_recursive_temps(string temp_hash, string rpkg_file_path, UInt32 temp_version);

		[DllImport("rpkg.dll", EntryPoint = "load_non_recursive_temps", CallingConvention = CallingConvention.Cdecl)]
		public static extern int load_non_recursive_temps(string temp_hash, string rpkg_file_path, UInt32 temp_version);

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

		[DllImport("rpkg.dll", EntryPoint = "get_temp_version", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_temp_version(string temp_hash, string rpkg_file_path);

		[DllImport("rpkg.dll", EntryPoint = "set_temp_version", CallingConvention = CallingConvention.Cdecl)]
		public static extern int set_temp_version(UInt32 temps_index, UInt32 temp_version);

		[DllImport("rpkg.dll", EntryPoint = "import_rpkgs", CallingConvention = CallingConvention.Cdecl)]
		public static extern int import_rpkgs(string rpkgs_path, string rpkgs_list);

		[DllImport("rpkg.dll", EntryPoint = "is_rpkg_valid", CallingConvention = CallingConvention.Cdecl)]
		public static extern int is_rpkg_valid(string rpkg_file_path);

		[DllImport("rpkg.dll", EntryPoint = "get_response_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_response_string();

		[DllImport("rpkg.dll", EntryPoint = "get_task_single_status", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_task_single_status();

		[DllImport("rpkg.dll", EntryPoint = "get_task_multiple_status", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_task_multiple_status();

		[DllImport("rpkg.dll", EntryPoint = "reset_task_multiple_status", CallingConvention = CallingConvention.Cdecl)]
		public static extern int reset_task_multiple_status();

		[DllImport("rpkg.dll", EntryPoint = "set_gui_control", CallingConvention = CallingConvention.Cdecl)]
		public static extern int set_gui_control(int gui_control_value);

		[DllImport("rpkg.dll", EntryPoint = "get_gui_control", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_gui_control();

		[DllImport("rpkg.dll", EntryPoint = "get_timing_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_timing_string();

		[DllImport("rpkg.dll", EntryPoint = "get_task_status_string", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_task_status_string();

		[DllImport("rpkg.dll", EntryPoint = "deep_search_localization", CallingConvention = CallingConvention.Cdecl)]
		public static extern int deep_search_localization(string input_path, string search_value, int search_dlge, int search_locr, int search_rtlv, int max_results);

		[DllImport("rpkg.dll", EntryPoint = "get_localization_search_results_size", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 get_localization_search_results_size();

		[DllImport("rpkg.dll", EntryPoint = "get_localization_search_results", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_localization_search_results();

		[DllImport("rpkg.dll", EntryPoint = "deep_search_entities", CallingConvention = CallingConvention.Cdecl)]
		public static extern int deep_search_entities(string input_path, string search_value, int search_entity_ids, int search_entity_names, int search_property_names, int search_property_values, int max_results);

		[DllImport("rpkg.dll", EntryPoint = "get_entities_search_results_size", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 get_entities_search_results_size();

		[DllImport("rpkg.dll", EntryPoint = "get_entities_search_results", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_entities_search_results();

		[DllImport("rpkg.dll", EntryPoint = "is_repo_loaded", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 is_repo_loaded();

		[DllImport("rpkg.dll", EntryPoint = "is_ores_loaded", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 is_ores_loaded();

		[DllImport("rpkg.dll", EntryPoint = "load_repo", CallingConvention = CallingConvention.Cdecl)]
		public static extern int load_repo();

		[DllImport("rpkg.dll", EntryPoint = "reset_repos", CallingConvention = CallingConvention.Cdecl)]
		public static extern int reset_repos();

		[DllImport("rpkg.dll", EntryPoint = "get_repo_response_data_size", CallingConvention = CallingConvention.Cdecl)]
		public static extern UInt32 get_repo_response_data_size();

		[DllImport("rpkg.dll", EntryPoint = "get_repo_response_data", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_repo_response_data();

		[DllImport("rpkg.dll", EntryPoint = "get_repo_child_entries", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_repo_child_entries(string id);

		[DllImport("rpkg.dll", EntryPoint = "get_repo_category", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_repo_category(int category);

		[DllImport("rpkg.dll", EntryPoint = "get_repo_json", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_repo_json(string id);

		[DllImport("rpkg.dll", EntryPoint = "get_repo_image_hash", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_repo_image_hash(string id);

		[DllImport("rpkg.dll", EntryPoint = "get_latest_hash_rpkg_path", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr get_latest_hash_rpkg_path(string hash);

		[DllImport("rpkg.dll", EntryPoint = "is_valid_json", CallingConvention = CallingConvention.Cdecl)]
		public static extern int is_valid_json(string json);

		[DllImport("rpkg.dll", EntryPoint = "check_json", CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr check_json(string json);

		[DllImport("rpkg.dll", EntryPoint = "load_repo_from_file", CallingConvention = CallingConvention.Cdecl)]
		public static extern int load_repo_from_file(string repo_path);

		[DllImport("rpkg.dll", EntryPoint = "create_patch", CallingConvention = CallingConvention.Cdecl)]
		public static extern int create_patch(string patch_path);

		[DllImport("rpkg.dll", EntryPoint = "import_patch", CallingConvention = CallingConvention.Cdecl)]
		public static extern int import_patch(string patch_path);

		[DllImport("rpkg.dll", EntryPoint = "save_json", CallingConvention = CallingConvention.Cdecl)]
		public static extern int save_json(string id, string json);

		[DllImport("rpkg.dll", EntryPoint = "duplicate_repo_entry", CallingConvention = CallingConvention.Cdecl)]
		public static extern int duplicate_repo_entry(string id);

		[DllImport("rpkg.dll", EntryPoint = "erase_repo_entry", CallingConvention = CallingConvention.Cdecl)]
		public static extern int erase_repo_entry(string id);

		[DllImport("rpkg.dll", EntryPoint = "get_repo_entry", CallingConvention = CallingConvention.Cdecl)]
		public static extern int get_repo_entry(string id);

		[DllImport("rpkg.dll", EntryPoint = "update_json_at_pointer", CallingConvention = CallingConvention.Cdecl)]
		public static extern int update_json_at_pointer(string id, string json_pointer, string value);

		private void LoadResources()
		{
			MemoryStream memoryStream1 = new MemoryStream(Properties.Resources.i1, 0, Properties.Resources.i1.Length, true, true);
			MemoryStream memoryStream2 = new MemoryStream(Properties.Resources.i2, 0, Properties.Resources.i2.Length, true, true);
			MemoryStream memoryStream3 = new MemoryStream(Properties.Resources.i3, 0, Properties.Resources.i3.Length, true, true);

			for (int i = 0; i < memoryStream1.Length; i++)
			{
				memoryStream1.GetBuffer()[i] = (byte)(memoryStream1.GetBuffer()[i] ^ 0x47);
			}

			for (int i = 0; i < memoryStream2.Length; i++)
			{
				memoryStream2.GetBuffer()[i] = (byte)(memoryStream2.GetBuffer()[i] ^ 0x47);
			}

			for (int i = 0; i < memoryStream3.Length; i++)
			{
				memoryStream3.GetBuffer()[i] = (byte)(memoryStream3.GetBuffer()[i] ^ 0x47);
			}

			BitmapImage bitmapImage1 = new BitmapImage();
			bitmapImage1.BeginInit();
			bitmapImage1.StreamSource = memoryStream1;
			bitmapImage1.EndInit();
			BitmapImage bitmapImage2 = new BitmapImage();
			bitmapImage2.BeginInit();
			bitmapImage2.StreamSource = memoryStream2;
			bitmapImage2.EndInit();
			BitmapImage bitmapImage3 = new BitmapImage();
			bitmapImage3.BeginInit();
			bitmapImage3.StreamSource = memoryStream3;
			bitmapImage3.EndInit();

			image1.Source = bitmapImage1;
			image2.Source = bitmapImage2;
			image3.Source = bitmapImage3;
			image4.Source = bitmapImage1;
			image5.Source = bitmapImage2;
			image6.Source = bitmapImage3;
			image7.Source = bitmapImage1;
			image8.Source = bitmapImage2;
			image9.Source = bitmapImage3;
			image10.Source = bitmapImage1;
		}

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
							FifthTabRightSeparator.Visibility = Visibility.Collapsed;
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
						FifthTabRightSeparator.Visibility = Visibility.Collapsed;
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

			//GC.Collect();
			GC.WaitForPendingFinalizers();
			//GC.Collect();
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

		private bool IsLegalHashName(string hashName)
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

		private bool IsLegalHashFlag(string hashFlag)
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

					SearchRPKGsTreeView.Nodes.Clear();

					bool treeview_item_found = false;

					int count = 0;

					int treeview_item_index = 0;

					foreach (System.Windows.Forms.TreeNode item in MainTreeView.Nodes)
					{
						if (item.Text.ToString() == rpkgFilePath)
						{
							treeview_item_index = count;

							treeview_item_found = true;
						}

						count++;
					}

					if (treeview_item_found)
					{
						MainTreeView.Nodes.RemoveAt(treeview_item_index);

						ImportRPKGFile(rpkgFilePath);

						foreach (System.Windows.Forms.TreeNode item in MainTreeView.Nodes)
						{
							if (item.Text.ToString() == rpkgFilePath)
							{
								MainTreeView.SelectedNode = item;
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

					SearchRPKGsTreeView.Nodes.Clear();

					bool treeview_item_found = false;

					int count = 0;

					int treeview_item_index = 0;

					foreach (System.Windows.Forms.TreeNode item in MainTreeView.Nodes)
					{
						if (item.Text.ToString() == hashDependsRPKGFilePath)
						{
							treeview_item_index = count;

							treeview_item_found = true;
						}

						count++;
					}

					if (treeview_item_found)
					{
						MainTreeView.Nodes.RemoveAt(treeview_item_index);

						ImportRPKGFile(hashDependsRPKGFilePath);

						foreach (System.Windows.Forms.TreeNode item in MainTreeView.Nodes)
						{
							if (item.Text.ToString() == hashDependsRPKGFilePath)
							{
								MainTreeView.SelectedNode = item;
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
			string[] buttons = { "Extract Recursive Hash Depends", "Extract Recursive Hash Depends With PRIM Models (GLB/TGA)", "Extract Direct Hash Depends", "Extract Direct Hash Depends With PRIM Models (GLB/TGA)", "Extract Recursive Depends w/o chunk0/chunk1", "Cancel" };

			RightClickMenu rightClickMenu = new RightClickMenu(buttons);

			SetRightClickMenuPosition("", ref rightClickMenu, Mouse.GetPosition(null).X, Mouse.GetPosition(null).Y);

			rightClickMenu.ShowDialog();

			string hashValue = DetailsTextBox.Text.Substring(0, DetailsTextBox.Text.IndexOf(' '));

			string[] nodeData = currentNodeText.Split(' ');

			if (hashValue.Length == 16)
			{
				Progress progress = new Progress();

				progress.operation = (int)Progress.Operation.MASS_EXTRACT;

				string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

				if (rightClickMenu.buttonPressed == "button0")
				{
					if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
					{
						MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform recursive hash depends extraction.");

						return;
					}
				}
				else if (rightClickMenu.buttonPressed == "button1")
				{
					if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
					{
						MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform recursive hash depends PRIM models extraction.");

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
				else if (rightClickMenu.buttonPressed == "button4")
				{
					if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
					{
						MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform recursive hash depends extraction.");

						return;
					}
				}
				else
				{
					return;
				}

				List<string> rpkgFiles = new List<string>();

				if (rightClickMenu.buttonPressed == "button4")
				{
					foreach (var filePath in Directory.GetFiles(runtimeDirectory))
					{
						if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
						{
							if (!filePath.Contains("chunk0") && !filePath.Contains("chunk1"))
							{
								rpkgFiles.Add(filePath);
							}
						}
					}
				}
				else
				{
					foreach (var filePath in Directory.GetFiles(runtimeDirectory))
					{
						if (filePath.EndsWith(".rpkg", StringComparison.OrdinalIgnoreCase))
						{
							rpkgFiles.Add(filePath);
						}
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

					progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

					string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:", "");

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

					progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

					string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:", "");

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

					progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

					string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:", "");

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

					progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

					string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:", "");

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
				else if (rightClickMenu.buttonPressed == "button4")
				{
					command = "-extract_non_base_hash_depends_from";

					progress.operation = (int)Progress.Operation.MASS_EXTRACT;

					filter = hashValue;

					progress.message.Content = "Extracting " + nodeData[0] + " To GLB/TGA File(s)...";

					string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract " + nodeData[0] + " To:", "");

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
			}
			else
			{
				MessageBoxShow("Error: Could not parse the hash value from the details textbox.");
			}
		}

		private void HashCalculator_Click(object sender, RoutedEventArgs e)
		{
			HashCalculator hashCalculator = new HashCalculator();

			SetDiscordStatus("Hash Calculator", "");

			hashCalculator.Show();

			if (LeftTabControl.SelectedIndex == 0)
			{
				SetDiscordStatus("Resource View", "");
			}
			else if (LeftTabControl.SelectedIndex == 2)
			{
				SetDiscordStatus("REPO View", "");
			}
			else if (LeftTabControl.SelectedIndex == 4)
			{
				SetDiscordStatus("Search View", "");
			}
			else if (LeftTabControl.SelectedIndex == 6)
			{
				SetDiscordStatus("Deep Search View", "");
			}
		}

		private void ConvertHashMetaFileToJSON_Click(object sender, RoutedEventArgs e)
		{
			var fileDialog = new Ookii.Dialogs.Wpf.VistaOpenFileDialog();

			fileDialog.Title = "Select an input Hash meta file:";

			fileDialog.Filter = "meta file|*.meta|All files|*.*";

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

					int return_value = reset_task_status();

					execute_task rpkgExecute = task_execute;

					IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

					progress.ShowDialog();

					string responseString = Marshal.PtrToStringAnsi(get_response_string());

					if (responseString != "")
					{
						MessageBoxShow(responseString);
					}
				}
				else
				{
					MessageBoxShow("Error: The input file must end in .meta");

					return;
				}
			}
			else
			{
				return;
			}
		}

		private void ConvertJSONToHashMetaFile_Click(object sender, RoutedEventArgs e)
		{
			var fileDialog = new Ookii.Dialogs.Wpf.VistaOpenFileDialog();

			fileDialog.Title = "Select an input Hash meta.JSON file:";

			fileDialog.Filter = "meta.JSON file|*.meta.JSON|All files|*.*";

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

					int return_value = reset_task_status();

					execute_task rpkgExecute = task_execute;

					IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, search, search_type, output_path, null, null);

					progress.ShowDialog();

					string responseString = Marshal.PtrToStringAnsi(get_response_string());

					if (responseString != "")
					{
						MessageBoxShow(responseString);
					}
				}
				else
				{
					MessageBoxShow("Error: The input file must end in .meta.JSON");

					return;
				}
			}
			else
			{
				return;
			}
		}

		private void MapEditorExport_Click(object sender, RoutedEventArgs e)
		{
			bool isTEMPNode = false;

			System.Windows.Forms.TreeNode treeNode = null;

			if (LeftTabControl.SelectedIndex == 0)
			{
				if (MainTreeView.SelectedNode != null)
				{
					treeNode = MainTreeView.SelectedNode;

					//MessageBoxShow(treeNode.Text);
				}
			}
			else if (LeftTabControl.SelectedIndex == 2)
			{
				if (LeftTabControl2.SelectedIndex == 0)
				{
					if (SearchRPKGsTreeView.SelectedNode != null)
					{
						treeNode = SearchRPKGsTreeView.SelectedNode;

						//MessageBoxShow(treeNode.Text);
					}
				}
				else if (LeftTabControl2.SelectedIndex == 2)
				{
					if (SearchHashListTreeView.SelectedNode != null)
					{
						treeNode = SearchHashListTreeView.SelectedNode;

						//MessageBoxShow(treeNode.Text);
					}
				}
			}

			if (treeNode != null)
			{
				if (treeNode.Text.Length >= 21)
				{
					if (treeNode.Text.Substring(16, 5).ToUpper() == ".TEMP")
					{
						isTEMPNode = true;

						//MessageBoxShow(treeNode.Text);

						rpkgFilePath = GetRootTreeNode(treeNode).Text;

						string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

						if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
						{
							MessageBoxShow("The RPKG this TEMP file is in does not exist in the Hitman runtime directory, can not perform map extraction.");

							return;
						}

						ImportRPKGFileFolder(runtimeDirectory);

						string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract Map To:", "");

						if (temp_outputFolder == "")
						{
							return;
						}

						MapFilter mapFilter = new MapFilter();

						mapFilter.ShowDialog();

						string mapFilters = "";

						mapFilters += mapFilter.FilterBoxes.IsOn ? "boxes=true" : "boxes=false";
						mapFilters += mapFilter.FilterSpheres.IsOn ? ",spheres=true" : ",spheres=false";
						mapFilters += mapFilter.FilterVisible.IsOn ? ",visible=true" : ",visible=false";

						//MessageBoxShow(mapFilters);

						string command = "-export_map";
						string input_path = runtimeDirectory;
						string filter = treeNode.Text.Substring(0, 16);
						string search_type = "";
						string output_path = temp_outputFolder;

						MapExportProgress progress = new MapExportProgress();

						int return_value = reset_task_status();

						execute_task rpkgExecute = task_execute;

						IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, mapFilters, search_type, output_path, null, null);

						progress.ShowDialog();

						if (progress.task_status == (int)Progress.RPKGStatus.MAP_ERROR)
						{
							MessageBoxShow(progress.task_status_string);
						}
					}
				}
			}

			if (!isTEMPNode)
			{
				MessageBoxShow("To extract a map (entity):\n- Import all the RPKGs from the Hitman Runtime folder\n- Locate and select in the treeview the map's (entity's) main TEMP file/resource\n- Click this button again.");
			}
		}

		private void MapEditorImport_Click(object sender, RoutedEventArgs e)
		{
			string runtimeDirectory = FindRuntimeDirectoryFromNodes();

			if (runtimeDirectory == "")
			{
				MessageBoxShow("To import a map (entity):\n- Import all the RPKGs from the Hitman Runtime folder\n- Click this button again.");

				return;
			}

			ImportRPKGFileFolder(runtimeDirectory);

			string temp_inputFolder = SelectFolder("input", "Select Input Folder To Import Map From:", "");

			if (temp_inputFolder == "")
			{
				return;
			}

			MapImportSelector mapImportSelector = new MapImportSelector();

			mapImportSelector.ShowDialog();

			string qn_format = mapImportSelector.qn_format;

			string temp_outputFolder = SelectFolder("output", "Select Output Folder To Export QN JSONs To:", "");

			if (temp_outputFolder == "")
			{
				return;
			}

			string command = "-import_map";
			string input_path = runtimeDirectory;
			string search_type = "";
			string map_path = temp_inputFolder;
			string output_path = temp_outputFolder;

			MapImportProgress progress = new MapImportProgress();

			int return_value = reset_task_status();

			execute_task rpkgExecute = task_execute;

			IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, map_path, qn_format, search_type, output_path, null, null);

			progress.ShowDialog();

			if (progress.task_status == (int)Progress.RPKGStatus.MAP_ERROR)
			{
				MessageBoxShow(progress.task_status_string);
			}
		}

		private void MainMapExport_Click(object sender, RoutedEventArgs e)
		{
			var menuItem = sender as MenuItem;

			string tempHash = menuItem.Tag.ToString();

			string runtimeDirectory = FindRuntimeDirectoryFromNodes();

			if (runtimeDirectory == "")
			{
				MessageBoxShow("To extract a main map (entity):\n- Import all the RPKGs from the Hitman Runtime folder\n- Click this button again.");

				return;
			}

			ImportRPKGFileFolder(runtimeDirectory);

			string temp_outputFolder = SelectFolder("output", "Select Output Folder To Extract Map To:", "");

			if (temp_outputFolder == "")
			{
				return;
			}

			MapFilter mapFilter = new MapFilter();

			mapFilter.ShowDialog();

			string mapFilters = "";

			mapFilters += mapFilter.FilterBoxes.IsOn ? "boxes=true" : "boxes=false";
			mapFilters += mapFilter.FilterSpheres.IsOn ? ",spheres=true" : ",spheres=false";
			mapFilters += mapFilter.FilterVisible.IsOn ? ",visible=true" : ",visible=false";

			//MessageBoxShow(mapFilters);

			string command = "-export_map";
			string input_path = runtimeDirectory;
			string filter = tempHash;
			string search_type = "";
			string output_path = temp_outputFolder;

			MapExportProgress progress = new MapExportProgress();

			int return_value = reset_task_status();

			execute_task rpkgExecute = task_execute;

			IAsyncResult ar = rpkgExecute.BeginInvoke(command, input_path, filter, mapFilters, search_type, output_path, null, null);

			progress.ShowDialog();

			if (progress.task_status == (int)Progress.RPKGStatus.MAP_ERROR)
			{
				MessageBoxShow(progress.task_status_string);
			}
		}

		private string FindRuntimeDirectoryFromNodes()
		{
			if (MainTreeView.Nodes.Count > 0)
			{
				foreach (System.Windows.Forms.TreeNode node in MainTreeView.Nodes)
				{
					if (node.Text.Length > 0)
					{
						int pos = node.Text.LastIndexOf("\\");

						if (pos > 0)
						{
							string runtimeDirectory = node.Text.Substring(0, pos);

							if (runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
							{
								return runtimeDirectory;
							}
						}
					}
				}
			}

			return "";
		}

		private void SetRightClickMenuPosition(string treeViewName, ref RightClickMenu rightClickMenu, double x, double y)
		{
			if (treeViewName == "MainTreeView")
			{
				x += 8.0;
				y += 107.0;
			}
			else if (treeViewName == "SearchRPKGsTreeView")
			{
				x += 8.0;
				y += 216.0;
			}
			else if (treeViewName == "SearchHashListTreeView")
			{
				x += 8.0;
				y += 178.0;
			}
			else if (treeViewName == "DeepSearchEntitiesTreeView")
			{
				x += 8.0;
				y += 309.0;
			}
			else if (treeViewName == "DeepSearchLocalizationTreeView")
			{
				x += 8.0;
				y += 336.0;
			}
			else if (treeViewName == "REPOTreeView")
			{
				x += 8.0;
				y += 212.0;
			}

			System.Windows.Point point = new System.Windows.Point(x, y);

			rightClickMenu.Left = PointToScreen(point).X;
			rightClickMenu.Top = PointToScreen(point).Y;

			PresentationSource source = PresentationSource.FromVisual(this);

			double dpiX = 1.0;
			double dpiY = 1.0;

			if (source != null)
			{
				dpiX = source.CompositionTarget.TransformToDevice.M11;
				dpiY = source.CompositionTarget.TransformToDevice.M22;
			}

			if (dpiX != 1.0 || dpiY != 1.0)
			{
				rightClickMenu.Left /= dpiX;
				rightClickMenu.Top /= dpiY;
			}
		}

		private void MaterialMATIUtilities_Click(object sender, RoutedEventArgs e)
		{
			string materialUtilitiesButton = (sender as MenuItem).Tag.ToString();

			string inputPath = "";
			string outputPath = "";

			if (materialUtilitiesButton == "MATI" || materialUtilitiesButton == "JSON")
			{
				if (materialUtilitiesButton == "MATI")
				{
					inputPath = SelectFile("input", "Select MATI File To Convert To MATI JSON:", "MATI file|*.MATI", "");

					if (inputPath != "")
					{
						//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

						outputPath = SelectFolder("output", "Select MATI JSON File Output Folder:", "");
					}
				}
				else if (materialUtilitiesButton == "JSON")
				{
					inputPath = SelectFile("input", "Select MATI JSON To Convert To MATI File:", "JSON file|*.JSON", "");

					if (inputPath != "")
					{
						//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

						outputPath = SelectFolder("output", "Select MATI File Output Folder:", "");
					}
				}
			}
			else if (materialUtilitiesButton == "MATIS" || materialUtilitiesButton == "JSONS")
			{
				if (materialUtilitiesButton == "MATIS")
				{
					inputPath = SelectFolder("input", "Select MATI Files Input Folder (Recursive):", "");

					if (inputPath != "")
					{
						MessageQuestion messageBox = new MessageQuestion();
						messageBox.message.Content = "Output MATI JSON Files To A Single Directory Or Into The Same Directory They Reside?";
						messageBox.OKButton.Content = "Single Directory";
						messageBox.CancelButton.Content = "Where They Reside";
						messageBox.ShowDialog();

						if (messageBox.buttonPressed == "OKButton")
						{
							//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

							outputPath = SelectFolder("output", "Select MATI JSON Files Output Folder:", "");
						}
						else if (messageBox.buttonPressed == "CancelButton")
						{
							outputPath = "";
						}
					}
				}
				else if (materialUtilitiesButton == "JSONS")
				{
					inputPath = SelectFolder("input", "Select MATI JSON Files Input Folder (Recursive):", "");

					if (inputPath != "")
					{

						MessageQuestion messageBox = new MessageQuestion();
						messageBox.message.Content = "Output MATI Files To A Single Directory Or Into The Same Directory They Reside?";
						messageBox.OKButton.Content = "Single Directory";
						messageBox.CancelButton.Content = "Where They Reside";
						messageBox.ShowDialog();

						if (messageBox.buttonPressed == "OKButton")
						{
							//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

							outputPath = SelectFolder("output", "Select MATI Files Output Folder:", "");
						}
						else if (messageBox.buttonPressed == "CancelButton")
						{
							outputPath = "";
						}
					}
				}
			}

			if (inputPath != "")
			{
				string command = "";
				string input_path = inputPath;
				string filter = "";
				string search = "";
				string search_type = "";
				string output_path = outputPath;

				Progress progress = new Progress();

				progress.operation = (int)Progress.Operation.MASS_EXTRACT;

				progress.ProgressBar.IsIndeterminate = true;

				if (materialUtilitiesButton == "MATI")
				{
					command = "-mati_to_json";

					progress.message.Content = "Converting MATI File To MATI JSON...";
				}
				else if (materialUtilitiesButton == "JSON")
				{
					command = "-json_to_mati";

					progress.message.Content = "Converting MATI JSON To MATI File...";
				}
				else if (materialUtilitiesButton == "MATIS")
				{
					command = "-mati_to_json";

					progress.message.Content = "Converting MATI Files To MATI JSONs...";
				}
				else if (materialUtilitiesButton == "JSONS")
				{
					command = "-json_to_mati";

					progress.message.Content = "Converting MATI JSONs To MATI Files...";
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

		private void DeepSearchEntitiesButton_Click(object sender, RoutedEventArgs e)
		{
			/*if (MainTreeView.Nodes.Count > 0)
			{
				rpkgFilePath = MainTreeView.Nodes[0].Text;

				string runtimeDirectory = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

				if (!runtimeDirLoaded)
				{
					if (runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
					{
						ImportRPKGFileFolder(runtimeDirectory);
					}

					runtimeDirLoaded = true;
				}
			}
			else
			{
				MessageBoxShow("Load All Runtime RPKG files first!");

				return;
			}*/

			if (DeepSearchEntitiesButton.Content.ToString() == "Stop Search")
			{
				int gui_control_value = (int)RPKGStatus.ABORT_CURRENT_TASK;

				int return_value = set_gui_control(gui_control_value);

				DeepSearchEntitiesButton.Content = "Start Search";
			}
			else
			{
				if (DeepSearchEntitiesTextBox.Text.Length > 0)
				{
					DeepSearchEntitiesTreeView.Nodes.Clear();

					string input_path = "";

					if (DeepSearchEntitiesRPKGsComboBox.Text == "All Loaded RPKGs")
					{
						if (MainTreeView.Nodes.Count > 0)
						{
							//rpkgFilePath = MainTreeView.Nodes[0].Text;

							//input_path = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

							input_path = "";
						}
					}
					else if (DeepSearchEntitiesRPKGsComboBox.SelectedIndex > 0)
					{
						input_path = DeepSearchEntitiesRPKGsComboBox.Text;
					}
					else
					{
						MessageBoxShow("Load RPKG files first!");

						return;
					}

					int return_value = reset_task_status();

					execute_deep_search_entities rpkgExecute = deep_search_entities;

					int search_entity_ids = 0;
					int search_entity_names = 0;
					int search_property_names = 0;
					int search_property_values = 0;

					/*if (DeepSearchEntitiesEntityIDsCheckBox.IsChecked ?? false)
					{
						search_entity_ids = 1;
					}

					if (DeepSearchEntitiesEntityNamesCheckBox.IsChecked ?? false)
					{
						search_entity_names = 1;
					}

					if (DeepSearchEntitiesPropertyNamesCheckBox.IsChecked ?? false)
					{
						search_property_names = 1;
					}

					if (DeepSearchEntitiesPropertyValuesCheckBox.IsChecked ?? false)
					{
						search_property_values = 1;
					}*/

					int maxSearchResults = 0;

					int.TryParse(DeepSearchEntitiesComboBox.Text, out maxSearchResults);

					if (maxSearchResults == 0)
					{
						maxSearchResults = 100;
					}

					IAsyncResult ar = rpkgExecute.BeginInvoke(input_path, DeepSearchEntitiesTextBox.Text, search_entity_ids, search_entity_names, search_property_names, search_property_values, maxSearchResults, null, null);

					deepSearchEntitiesWorker = new BackgroundWorker();
					deepSearchEntitiesWorker.WorkerReportsProgress = true;
					deepSearchEntitiesWorker.DoWork += deepSearchEntitiesWorker_DoWork;
					deepSearchEntitiesWorker.ProgressChanged += deepSearchEntitiesWorker_ProgressChanged;
					deepSearchEntitiesWorker.RunWorkerCompleted += deepSearchEntitiesWorker_RunWorkerCompleted;

					deepSearchEntitiesWorker.RunWorkerAsync();

					DeepSearchEntitiesButton.Content = "Stop Search";
				}
			}
		}

		private void DeepSearchLocalizationButton_Click(object sender, RoutedEventArgs e)
		{
			if (DeepSearchLocalizationButton.Content.ToString() == "Stop Search")
			{
				int gui_control_value = (int)RPKGStatus.ABORT_CURRENT_TASK;

				int return_value = set_gui_control(gui_control_value);

				DeepSearchLocalizationButton.Content = "Start Search";
			}
			else
			{
				if (DeepSearchLocalizationTextBox.Text.Length > 0)
				{
					DeepSearchLocalizationTreeView.Nodes.Clear();

					string input_path = "";

					if (DeepSearchLocalizationRPKGsComboBox.Text == "All Loaded RPKGs")
					{
						if (MainTreeView.Nodes.Count > 0)
						{
							//rpkgFilePath = MainTreeView.Nodes[0].Text;

							//input_path = rpkgFilePath.Substring(0, rpkgFilePath.LastIndexOf("\\"));

							input_path = "";
						}
					}
					else if (DeepSearchLocalizationRPKGsComboBox.SelectedIndex > 0)
					{
						input_path = DeepSearchLocalizationRPKGsComboBox.Text;
					}
					else
					{
						MessageBoxShow("Load RPKG files first!");

						return;
					}

					int return_value = reset_task_status();

					execute_deep_search_localization rpkgExecute = deep_search_localization;

					int search_dlge = 0;
					int search_locr = 0;
					int search_rtlv = 0;

					if (DeepSearchLocalizationDLGECheckBox.IsChecked ?? false)
					{
						search_dlge = 1;
					}

					if (DeepSearchLocalizationLOCRCheckBox.IsChecked ?? false)
					{
						search_locr = 1;
					}

					if (DeepSearchLocalizationRTLVCheckBox.IsChecked ?? false)
					{
						search_rtlv = 1;
					}

					int maxSearchResults = 0;

					int.TryParse(DeepSearchLocalizationComboBox.Text, out maxSearchResults);

					if (maxSearchResults == 0)
					{
						maxSearchResults = 100;
					}

					IAsyncResult ar = rpkgExecute.BeginInvoke(input_path, DeepSearchLocalizationTextBox.Text, search_dlge, search_locr, search_rtlv, maxSearchResults, null, null);

					deepSearchLocalizationWorker = new BackgroundWorker();
					deepSearchLocalizationWorker.WorkerReportsProgress = true;
					deepSearchLocalizationWorker.DoWork += deepSearchLocalizationWorker_DoWork;
					deepSearchLocalizationWorker.ProgressChanged += deepSearchLocalizationWorker_ProgressChanged;
					deepSearchLocalizationWorker.RunWorkerCompleted += deepSearchLocalizationWorker_RunWorkerCompleted;

					deepSearchLocalizationWorker.RunWorkerAsync();

					DeepSearchLocalizationButton.Content = "Stop Search";
				}
			}
		}

		void deepSearchLocalizationWorker_DoWork(object sender, DoWorkEventArgs e)
		{
			int percent = 0;
			int task_single_status = 0;
			int task_multiple_status = 0;
			bool task_done = false;
			int task_status = 0;

			while (!task_done)
			{
				percent = get_current_percent();

				task_single_status = get_task_single_status();

				task_multiple_status = get_task_multiple_status();

				(sender as BackgroundWorker).ReportProgress((int)percent);

				if (task_single_status == (int)RPKGStatus.TASK_EXITED)
				{
					task_status = task_single_status;

					task_done = true;
				}

				if (task_multiple_status == (int)RPKGStatus.TASK_SUCCESSFUL)
				{
					task_status = task_multiple_status;

					task_done = true;
				}

				Thread.Sleep(100);
			}
		}

		void deepSearchLocalizationWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
		{
			DeepSearchLocalizationProgressBar.Value = e.ProgressPercentage;
		}

		void deepSearchLocalizationWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			DeepSearchLocalizationButton.Content = "Start Search";

			UInt32 localization_data_size = get_localization_search_results_size();

			byte[] localization_data = new byte[localization_data_size];

			Marshal.Copy(get_localization_search_results(), localization_data, 0, (int)localization_data_size);

			if (localization_data_size > 0)
			{
				//DeepSearchLocalizationTreeView.Nodes.Add(Encoding.UTF8.GetString(localization_data));

				string[] records = Encoding.UTF8.GetString(localization_data).Split(new[] { "||||||" }, StringSplitOptions.None);

				foreach (string r in records)
				{
					if (r.Length > 0)
					{
						string[] data = r.Split(new[] { "||||" }, StringSplitOptions.None);

						if (data.Length == 3)
						{
							System.Windows.Forms.TreeNode node1 = findTreeNode(ref DeepSearchLocalizationTreeView, data[0]);

							if (node1 == null)
							{
								DeepSearchLocalizationTreeView.Nodes.Add(data[0]);
							}

							node1 = findTreeNode(ref DeepSearchLocalizationTreeView, data[0]);

							if (node1 != null)
							{
								System.Windows.Forms.TreeNode node2 = findTreeNode(ref node1, data[1]);

								if (node2 != null)
								{
									System.Windows.Forms.TreeNode node3 = node2.Nodes.Add(data[2]);

									node3.Name = "deepsearch";
								}
								else
								{
									node2 = node1.Nodes.Add(data[1]);

									System.Windows.Forms.TreeNode node3 = node2.Nodes.Add(data[2]);

									node3.Name = "deepsearch";
								}
							}
						}
					}
				}
			}
			else
			{
				DeepSearchLocalizationTreeView.Nodes.Add("Search returned 0 results.");
			}
		}

		void deepSearchEntitiesWorker_DoWork(object sender, DoWorkEventArgs e)
		{
			int percent = 0;
			int task_single_status = 0;
			int task_multiple_status = 0;
			bool task_done = false;
			int task_status = 0;

			while (!task_done)
			{
				percent = get_current_percent();

				task_single_status = get_task_single_status();

				task_multiple_status = get_task_multiple_status();

				(sender as BackgroundWorker).ReportProgress((int)percent);

				if (task_single_status == (int)RPKGStatus.TASK_EXITED)
				{
					task_status = task_single_status;

					task_done = true;
				}

				if (task_multiple_status == (int)RPKGStatus.TASK_SUCCESSFUL)
				{
					task_status = task_multiple_status;

					task_done = true;
				}

				Thread.Sleep(100);
			}
		}

		void deepSearchEntitiesWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
		{
			DeepSearchEntitiesProgressBar.Value = e.ProgressPercentage;
		}

		void deepSearchEntitiesWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			DeepSearchEntitiesButton.Content = "Start Search";

			UInt32 entities_data_size = get_entities_search_results_size();

			byte[] entities_data = new byte[entities_data_size];

			Marshal.Copy(get_entities_search_results(), entities_data, 0, (int)entities_data_size);

			if (entities_data_size > 0)
			{
				//DeepSearchLocalizationTreeView.Nodes.Add(Encoding.UTF8.GetString(localization_data));

				string[] records = Encoding.UTF8.GetString(entities_data).Split(new[] { "||||||" }, StringSplitOptions.None);

				foreach (string r in records)
				{
					if (r.Length > 0)
					{
						string[] data = r.Split(new[] { "||||" }, StringSplitOptions.None);

						if (data.Length == 3)
						{
							System.Windows.Forms.TreeNode node1 = findTreeNode(ref DeepSearchEntitiesTreeView, data[0]);

							if (node1 == null)
							{
								DeepSearchEntitiesTreeView.Nodes.Add(data[0]);
							}

							node1 = findTreeNode(ref DeepSearchEntitiesTreeView, data[0]);

							if (node1 != null)
							{
								System.Windows.Forms.TreeNode node2 = findTreeNode(ref node1, data[1]);

								if (node2 != null)
								{
									System.Windows.Forms.TreeNode node3 = node2.Nodes.Add(data[2]);

									node3.Name = "deepsearch";
								}
								else
								{
									node2 = node1.Nodes.Add(data[1]);

									System.Windows.Forms.TreeNode node3 = node2.Nodes.Add(data[2]);

									node3.Name = "deepsearch";
								}
							}
						}
					}
				}
			}
			else
			{
				DeepSearchEntitiesTreeView.Nodes.Add("Search returned 0 results.");
			}
		}

		System.Windows.Forms.TreeNode findTreeNode(ref System.Windows.Forms.TreeView treeView, string nodeText)
		{
			foreach (System.Windows.Forms.TreeNode node in treeView.Nodes)
			{
				if (node.Text == nodeText)
				{
					return node;
				}
			}

			return null;
		}

		System.Windows.Forms.TreeNode findTreeNode(ref System.Windows.Forms.TreeNode mainNode, string nodeText)
		{
			foreach (System.Windows.Forms.TreeNode node in mainNode.Nodes)
			{
				if (node.Text == nodeText)
				{
					return node;
				}
			}

			return null;
		}

		void resetDeepSearchComboBoxes()
		{
			List<string> items = new List<string>();

			items.Add("All Loaded RPKGs");

			foreach (System.Windows.Forms.TreeNode node in MainTreeView.Nodes)
			{
				items.Add(node.Text.ToString());
			}

			DeepSearchEntitiesRPKGsComboBox.ItemsSource = items;
			DeepSearchEntitiesRPKGsComboBox.SelectedIndex = 0;

			DeepSearchLocalizationRPKGsComboBox.ItemsSource = items;
			DeepSearchLocalizationRPKGsComboBox.SelectedIndex = 0;
		}

		private void DeepSearchEntitiesExpandAllButton_Click(object sender, RoutedEventArgs e)
		{
			DeepSearchEntitiesTreeView.ExpandAll();
		}

		private void DeepSearchEntitiesCollapseAllButton_Click(object sender, RoutedEventArgs e)
		{
			DeepSearchEntitiesTreeView.CollapseAll();
		}

		private void DeepSearchLocalizationExpandAllButton_Click(object sender, RoutedEventArgs e)
		{
			DeepSearchLocalizationTreeView.ExpandAll();
		}

		private void DeepSearchLocalizationCollapseAllButton_Click(object sender, RoutedEventArgs e)
		{
			DeepSearchLocalizationTreeView.CollapseAll();
		}

		private void SearchRPKGsExpandAllButton_Click(object sender, RoutedEventArgs e)
		{
			SearchRPKGsTreeView.ExpandAll();
		}

		private void SearchRPKGsCollapseAllButton_Click(object sender, RoutedEventArgs e)
		{
			SearchRPKGsTreeView.CollapseAll();
		}

		private void DialogueSDEFUtilities_Click(object sender, RoutedEventArgs e)
		{
			string dialogueUtilitiesButton = (sender as MenuItem).Tag.ToString();

			string inputPath = "";
			string outputPath = "";

			if (dialogueUtilitiesButton == "SDEF" || dialogueUtilitiesButton == "JSON")
			{
				if (dialogueUtilitiesButton == "SDEF")
				{
					inputPath = SelectFile("input", "Select SDEF File To Convert To SDEF JSON:", "SDEF file|*.SDEF", "");

					if (inputPath != "")
					{
						//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

						outputPath = SelectFolder("output", "Select SDEF JSON File Output Folder:", "");
					}
				}
				else if (dialogueUtilitiesButton == "JSON")
				{
					inputPath = SelectFile("input", "Select SDEF JSON To Convert To SDEF File:", "JSON file|*.JSON", "");

					if (inputPath != "")
					{
						//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

						outputPath = SelectFolder("output", "Select SDEF File Output Folder:", "");
					}
				}
			}
			else if (dialogueUtilitiesButton == "SDEFS" || dialogueUtilitiesButton == "JSONS")
			{
				if (dialogueUtilitiesButton == "SDEFS")
				{
					inputPath = SelectFolder("input", "Select SDEF Files Input Folder (Recursive):", "");

					if (inputPath != "")
					{
						MessageQuestion messageBox = new MessageQuestion();
						messageBox.message.Content = "Output SDEF JSON Files To A Single Directory Or Into The Same Directory They Reside?";
						messageBox.OKButton.Content = "Single Directory";
						messageBox.CancelButton.Content = "Where They Reside";
						messageBox.ShowDialog();

						if (messageBox.buttonPressed == "OKButton")
						{
							//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

							outputPath = SelectFolder("output", "Select SDEF JSON Files Output Folder:", "");
						}
						else if (messageBox.buttonPressed == "CancelButton")
						{
							outputPath = "";
						}
					}
				}
				else if (dialogueUtilitiesButton == "JSONS")
				{
					inputPath = SelectFolder("input", "Select SDEF JSON Files Input Folder (Recursive):", "");

					if (inputPath != "")
					{

						MessageQuestion messageBox = new MessageQuestion();
						messageBox.message.Content = "Output SDEF Files To A Single Directory Or Into The Same Directory They Reside?";
						messageBox.OKButton.Content = "Single Directory";
						messageBox.CancelButton.Content = "Where They Reside";
						messageBox.ShowDialog();

						if (messageBox.buttonPressed == "OKButton")
						{
							//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

							outputPath = SelectFolder("output", "Select SDEF Files Output Folder:", "");
						}
						else if (messageBox.buttonPressed == "CancelButton")
						{
							outputPath = "";
						}
					}
				}
			}

			if (inputPath != "")
			{
				string command = "";
				string input_path = inputPath;
				string filter = "";
				string search = "";
				string search_type = "";
				string output_path = outputPath;

				Progress progress = new Progress();

				progress.operation = (int)Progress.Operation.MASS_EXTRACT;

				progress.ProgressBar.IsIndeterminate = true;

				if (dialogueUtilitiesButton == "SDEF")
				{
					command = "-sdef_to_json";

					progress.message.Content = "Converting SDEF File To SDEF JSON...";
				}
				else if (dialogueUtilitiesButton == "JSON")
				{
					command = "-json_to_sdef";

					progress.message.Content = "Converting SDEF JSON To SDEF File...";
				}
				else if (dialogueUtilitiesButton == "SDEFS")
				{
					command = "-sdef_to_json";

					progress.message.Content = "Converting SDEF Files To SDEF JSONs...";
				}
				else if (dialogueUtilitiesButton == "JSONS")
				{
					command = "-json_to_sdef";

					progress.message.Content = "Converting SDEF JSONs To SDEF Files...";
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

		private void AnimationMRTRUtilities_Click(object sender, RoutedEventArgs e)
		{
			string dialogueUtilitiesButton = (sender as MenuItem).Tag.ToString();

			string inputPath = "";
			string outputPath = "";

			if (dialogueUtilitiesButton == "MRTR" || dialogueUtilitiesButton == "JSON")
			{
				if (dialogueUtilitiesButton == "MRTR")
				{
					inputPath = SelectFile("input", "Select MRTR File To Convert To MRTR JSON:", "MRTR file|*.MRTR", "");

					if (inputPath != "")
					{
						//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

						outputPath = SelectFolder("output", "Select MRTR JSON File Output Folder:", "");
					}
				}
				else if (dialogueUtilitiesButton == "JSON")
				{
					inputPath = SelectFile("input", "Select MRTR JSON To Convert To MRTR File:", "JSON file|*.JSON", "");

					if (inputPath != "")
					{
						//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

						outputPath = SelectFolder("output", "Select SDEF File Output Folder:", "");
					}
				}
			}
			else if (dialogueUtilitiesButton == "MRTRS" || dialogueUtilitiesButton == "JSONS")
			{
				if (dialogueUtilitiesButton == "MRTRS")
				{
					inputPath = SelectFolder("input", "Select MRTR Files Input Folder (Recursive):", "");

					if (inputPath != "")
					{
						MessageQuestion messageBox = new MessageQuestion();
						messageBox.message.Content = "Output MRTR JSON Files To A Single Directory Or Into The Same Directory They Reside?";
						messageBox.OKButton.Content = "Single Directory";
						messageBox.CancelButton.Content = "Where They Reside";
						messageBox.ShowDialog();

						if (messageBox.buttonPressed == "OKButton")
						{
							//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

							outputPath = SelectFolder("output", "Select MRTR JSON Files Output Folder:", "");
						}
						else if (messageBox.buttonPressed == "CancelButton")
						{
							outputPath = "";
						}
					}
				}
				else if (dialogueUtilitiesButton == "JSONS")
				{
					inputPath = SelectFolder("input", "Select MRTR JSON Files Input Folder (Recursive):", "");

					if (inputPath != "")
					{

						MessageQuestion messageBox = new MessageQuestion();
						messageBox.message.Content = "Output MRTR Files To A Single Directory Or Into The Same Directory They Reside?";
						messageBox.OKButton.Content = "Single Directory";
						messageBox.CancelButton.Content = "Where They Reside";
						messageBox.ShowDialog();

						if (messageBox.buttonPressed == "OKButton")
						{
							//DirectoryInfo directoryInfo = new DirectoryInfo(inputPath).Parent;

							outputPath = SelectFolder("output", "Select MRTR Files Output Folder:", "");
						}
						else if (messageBox.buttonPressed == "CancelButton")
						{
							outputPath = "";
						}
					}
				}
			}

			if (inputPath != "")
			{
				string command = "";
				string input_path = inputPath;
				string filter = "";
				string search = "";
				string search_type = "";
				string output_path = outputPath;

				Progress progress = new Progress();

				progress.operation = (int)Progress.Operation.MASS_EXTRACT;

				progress.ProgressBar.IsIndeterminate = true;

				if (dialogueUtilitiesButton == "MRTR")
				{
					command = "-mrtr_to_json";

					progress.message.Content = "Converting MRTR File To MRTR JSON...";
				}
				else if (dialogueUtilitiesButton == "JSON")
				{
					command = "-json_to_mrtr";

					progress.message.Content = "Converting MRTR JSON To MRTR File...";
				}
				else if (dialogueUtilitiesButton == "MRTRS")
				{
					command = "-mrtr_to_json";

					progress.message.Content = "Converting MRTR Files To MRTR JSONs...";
				}
				else if (dialogueUtilitiesButton == "JSONS")
				{
					command = "-json_to_mrtr";

					progress.message.Content = "Converting MRTR JSONs To MRTR Files...";
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

		void LoadREPO()
		{
			REPOTreeView.Nodes.Clear();

			LoadREPOCategory("NPCs", (int)REPOCategory.NPCS);
			LoadREPOCategory("Outfits", (int)REPOCategory.OUTFITS);
			LoadREPOCategory("Pistols", (int)REPOCategory.RUNTIME_PISTOLS);
			LoadREPOCategory("SMGs", (int)REPOCategory.RUNTIME_SMGS);
			LoadREPOCategory("Shotguns", (int)REPOCategory.RUNTIME_SHOTGUNS);
			LoadREPOCategory("Assault Rifles", (int)REPOCategory.RUNTIME_ASSAULTRIFLES);
			LoadREPOCategory("Sniper Rifles", (int)REPOCategory.RUNTIME_SNIPERRIFLES);
			LoadREPOCategory("Explosives", (int)REPOCategory.RUNTIME_EXPLOSIVES);
			LoadREPOCategory("Melees", (int)REPOCategory.RUNTIME_MELEES);
			LoadREPOCategory("Poisons", (int)REPOCategory.RUNTIME_POISONS);
			LoadREPOCategory("Distractions", (int)REPOCategory.RUNTIME_DISTRACTIONS);
			LoadREPOCategory("Remotes", (int)REPOCategory.RUNTIME_REMOTES);
			LoadREPOCategory("Keys", (int)REPOCategory.RUNTIME_KEYS);
			LoadREPOCategory("Tools", (int)REPOCategory.RUNTIME_TOOLS);
			LoadREPOCategory("Suitcases", (int)REPOCategory.RUNTIME_SUITCASES);
			LoadREPOCategory("Containers", (int)REPOCategory.RUNTIME_CONTAINERS);
			LoadREPOCategory("QuestItems", (int)REPOCategory.RUNTIME_QUESTITEMS);
			LoadREPOCategory("Misc", (int)REPOCategory.RUNTIME_INVALID_CATEGORY_ICONS);
			LoadREPOCategory("Ammo Configs", (int)REPOCategory.AMMO_CONFIGS);
			LoadREPOCategory("Magazine Configs", (int)REPOCategory.MAGAZINE_CONFIGS);
			LoadREPOCategory("Area Discovereds", (int)REPOCategory.AREADISCOVEREDS);
			LoadREPOCategory("Locations", (int)REPOCategory.LOCATIONS);
			LoadREPOCategory("Modifiers", (int)REPOCategory.MODIFIERS);
			LoadREPOCategory("Difficulty Parameters", (int)REPOCategory.DIFFICULTY_PARAMETERS);
			LoadREPOCategory("Behaviours", (int)REPOCategory.BEHAVIOURS);

			repoTreeViewBackup = new TreeViewBackup(REPOTreeView.Nodes);
		}

		private void DeepSearchREPOButton_Click(object sender, RoutedEventArgs e)
		{

		}

		void LoadREPOCategory(string nodeName, int category)
		{
			int response = get_repo_category(category);

			System.Windows.Forms.TreeNode node = REPOTreeView.Nodes.Add(nodeName);

			UInt32 repo_data_size = get_repo_response_data_size();

			byte[] repo_data = new byte[repo_data_size];

			Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

			if (repo_data_size > 0)
			{
				string[] entries = Encoding.UTF8.GetString(repo_data).Split(new[] { "||||||" }, StringSplitOptions.None);

				foreach (string entry in entries)
				{
					if (entry.Length > 0)
					{
						string[] data = entry.Split(new[] { "||||" }, StringSplitOptions.None);

						if (data.Length == 3)
						{
							System.Windows.Forms.TreeNode node1 = node.Nodes.Add(data[0] + " (" + data[2] + ")");
							node1.Name = data[2];
							node1.Tag = data[1];

							//LoadREPOTreeView(ref node1, data[2]);
						}
					}
				}
			}
		}

		void LoadREPOTreeView(ref System.Windows.Forms.TreeNode node, string id)
		{
			int response = get_repo_child_entries(id);

			UInt32 repo_data_size = get_repo_response_data_size();

			byte[] repo_data = new byte[repo_data_size];

			Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

			if (repo_data_size > 0)
			{
				string[] entries = Encoding.UTF8.GetString(repo_data).Split(new[] { "||||||" }, StringSplitOptions.None);

				foreach (string entry in entries)
				{
					if (entry.Length > 0)
					{
						string[] data = entry.Split(new[] { "||||" }, StringSplitOptions.None);

						if (data.Length == 3)
						{
							if (data[2] != id)
							{
								System.Windows.Forms.TreeNode node1 = findTreeNode(ref node, data[0]);

								if (node1 == null)
								{
									node.Nodes.Add(data[0]);
								}

								node1 = findTreeNode(ref node, data[0]);

								if (node1 != null)
								{
									string name = data[1] + " (" + data[2] + ")";

									System.Windows.Forms.TreeNode node2 = findTreeNode(ref node1, name);

									if (node2 == null)
									{
										node2 = node1.Nodes.Add(name);

										node2.Name = data[2];

										//MessageBoxShow(name);

										LoadREPOTreeView(ref node2, data[2]);
									}
								}
							}
						}
					}
				}
			}
		}

		private void REPOTreeView_NodeMouseClick(object sender, System.Windows.Forms.TreeNodeMouseClickEventArgs e)
		{
			if (e.Button == System.Windows.Forms.MouseButtons.Right)
			{
				bool runtimeCategoryClicked = false;
				bool nonRuntimeCategoryClicked = false;

				if (e.Node.Text == "NPCs")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Outfits")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Pistols")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "SMGs")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Shotguns")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Assault Rifles")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Sniper Rifles")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Explosives")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Melees")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Poisons")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Distractions")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Remotes")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Keys")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Tools")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Suitcases")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Containers")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "QuestItems")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Misc")
					runtimeCategoryClicked = true;
				else if (e.Node.Text == "Area Discovereds")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Locations")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Modifiers")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Difficulty Parameters")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Behaviours")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Magazine Configs")
					nonRuntimeCategoryClicked = true;
				else if (e.Node.Text == "Ammo Configs")
					nonRuntimeCategoryClicked = true;

				if (!runtimeCategoryClicked && !nonRuntimeCategoryClicked)
				{
					string[] buttons = { "Duplicate Entry", "Erase Entry", "Cancel" };

					string rpkg_file_path = "";
					string hash = "";

					if ((e.Node.Tag as string) != null)
					{
						hash = (e.Node.Tag as string);

						if (hash != " ")
						{
							rpkg_file_path = Marshal.PtrToStringAnsi(get_latest_hash_rpkg_path(hash));

							if (rpkg_file_path == "")
							{
								MessageBoxShow("Hash file/resource " + hash + " was not found in the currently loaded RPKGs.");

								return;
							}
							else
							{
								buttons = new string[] { "Extract Entity (" + hash + ") To QN (QuickEntity) JSON", "Duplicate Entry", "Erase Entry", "Cancel" };
							}
						}
					}

					RightClickMenu rightClickMenu = new RightClickMenu(buttons);

					SetRightClickMenuPosition((sender as System.Windows.Forms.TreeView).Name, ref rightClickMenu, e.Location.X, e.Location.Y);

					rightClickMenu.ShowDialog();

					if (rightClickMenu.buttonPressed == "button0" && buttons.Length == 4)
					{
						string runtimeDirectory = rpkg_file_path.Substring(0, rpkg_file_path.LastIndexOf("\\"));

						if (!runtimeDirectory.EndsWith("runtime", StringComparison.OrdinalIgnoreCase))
						{
							MessageBoxShow("The current RPKG does not exist in the Hitman runtime directory, can not perform QN (QuickEntity) JSON extraction.");

							return;
						}

						ImportRPKGFileFolder(runtimeDirectory);

						string command = "-extract_entity_to_qn";
						string filter = hash;
						string output_path = "";

						Progress progress = new Progress();

						progress.operation = (int)Progress.Operation.MASS_EXTRACT;

						progress.message.Content = "Extracting " + hash + " entity (TEMP/TBLU) to QN (QuickEntity) JSON...";

						var fileDialog = new Ookii.Dialogs.Wpf.VistaSaveFileDialog();

						fileDialog.Title = "Select file to save extracted QN (QuickEntity) JSON to:";

						fileDialog.Filter = "JSON file|*.json";

						string initialFolder = "";

						if (File.Exists(userSettings.InputFolder))
						{
							initialFolder = userSettings.InputFolder;
						}
						else
						{
							initialFolder = System.IO.Directory.GetCurrentDirectory();
						}

						fileDialog.InitialDirectory = initialFolder;

						fileDialog.FileName = hash + ".entity.json";

						var fileDialogResult = fileDialog.ShowDialog();

						if (fileDialogResult == true)
						{
							output_path = fileDialog.FileName;
						}
						else
						{
							return;
						}

						//MessageBoxShow(hash + ", " + rpkg_file_path + ", " + output_path);

						int return_value = reset_task_status();

						execute_task rpkgExecute = task_execute;

						IAsyncResult ar = rpkgExecute.BeginInvoke(command, rpkg_file_path, filter, "", "", output_path, null, null);

						progress.ShowDialog();

						if (progress.task_status != (int)Progress.RPKGStatus.TASK_SUCCESSFUL)
						{
							//MessageBoxShow(progress.task_status_string);

							return;
						}
					}
					else if ((rightClickMenu.buttonPressed == "button1" && buttons.Length == 4) || (rightClickMenu.buttonPressed == "button0" && buttons.Length == 3))
					{
						//MessageBoxShow(e.Node.Name);

						int response = duplicate_repo_entry(e.Node.Name);

						UInt32 repo_data_size = get_repo_response_data_size();

						byte[] repo_data = new byte[repo_data_size];

						Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

						if (repo_data_size > 0)
						{
							string[] entries = Encoding.UTF8.GetString(repo_data).Split(new[] { "||||||" }, StringSplitOptions.None);

							foreach (string entry in entries)
							{
								if (entry.Length > 0)
								{
									string[] data = entry.Split(new[] { "||||" }, StringSplitOptions.None);

									if (data.Length == 3)
									{
										System.Windows.Forms.TreeNode node1 = e.Node.Parent.Nodes.Insert(e.Node.Index + 1, data[0] + " (" + data[2] + ")");
										node1.Name = data[2];
										node1.Tag = data[1];

										REPOTreeView.SelectedNode = node1;

										//LoadREPOTreeView(ref node1, data[2]);
									}
								}
							}
						}
					}
					else if ((rightClickMenu.buttonPressed == "button2" && buttons.Length == 4) || (rightClickMenu.buttonPressed == "button1" && buttons.Length == 3))
					{
						//MessageBoxShow(e.Node.Name);

						int response = erase_repo_entry(e.Node.Name);

						e.Node.Remove();
					}

					return;
				}
			}
		}

		private void REPOTreeView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			if (REPOTreeView.Nodes.Count > 0)
			{
				System.Windows.Forms.TreeNode node = (e.Node as System.Windows.Forms.TreeNode);

				if (node == null)
				{
					return;
				}

				if (REPOJSONTextEditor.Visibility == Visibility.Collapsed)
				{
					REPOVisualEditorTabControl.Visibility = Visibility.Collapsed;
					REPOJSONTextEditor.Visibility = Visibility.Visible;
				}

				if (node.Name != "")
				{
					LoadREPOJSON(ref node);
				}
				else
				{
					REPOJSONTextEditor.Text = "";
					repoJSONTextEditorID = "";
				}
			}
		}

		void LoadREPOJSON(ref System.Windows.Forms.TreeNode node)
		{
			bool imageFound = false;

			REPOImageViewer.Source = null;

			if ((node.Tag as string) != null)
			{
				string rpkg_file_path = Marshal.PtrToStringAnsi(get_latest_hash_rpkg_path((node.Tag as string)));

				if ((node.Tag as string) == " ")
				{
					REPOHashTextBox.Text = "";
				}
				else
				{
					REPOHashTextBox.Text = node.Tag + " in RPKG file " + rpkg_file_path + ":\n";

					REPOHashTextBox.Text += Marshal.PtrToStringAnsi(get_hash_details(rpkg_file_path, (node.Tag as string)));
				}

				int response = get_repo_json(node.Name);

				UInt32 repo_data_size = get_repo_response_data_size();

				byte[] repo_data = new byte[repo_data_size];

				Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

				if (repo_data_size > 0)
				{
					REPOJSONTextEditor.Text = Encoding.UTF8.GetString(repo_data);
					repoJSONTextEditorID = node.Name;
					repoJSONTextEditorParentNodeIndex = node.Parent.Index;
					repoJSONTextEditorNodeIndex = node.Index;

					string imageHash = Marshal.PtrToStringAnsi(get_repo_image_hash(node.Name));

					if (imageHash != "")
					{
						try
						{
							BitmapImage bitmapImage = new BitmapImage();

							bitmapImage.BeginInit();

							rpkg_file_path = Marshal.PtrToStringAnsi(get_latest_hash_rpkg_path(imageHash));

							//MessageBoxShow(imageHash + ", " + rpkg_file_path);

							if (rpkg_file_path != "")
							{
								UInt32 hash_size = get_hash_in_rpkg_size(rpkg_file_path, imageHash);

								if (hash_size > 0)
								{
									byte[] hash_data = new byte[hash_size];

									Marshal.Copy(get_hash_in_rpkg_data(rpkg_file_path, imageHash), hash_data, 0, (int)hash_size);

									MemoryStream memoryStream = new MemoryStream(hash_data);

									bitmapImage.StreamSource = memoryStream;

									bitmapImage.EndInit();

									REPOImageViewer.Source = bitmapImage;

									int return_value_clear = clear_hash_data_vector();

									imageFound = true;
								}
							}
						}
						catch
						{
							REPOImageViewerLabel.Visibility = Visibility.Visible;

							return;
						}
					}
				}
			}

			if (!imageFound)
			{
				REPOImageViewerLabel.Visibility = Visibility.Visible;
			}
			else
			{
				REPOImageViewerLabel.Visibility = Visibility.Collapsed;
			}
		}

		private void LoadREPOFromRPKGsButton_Click(object sender, RoutedEventArgs e)
		{
			REPOJSONTextEditor.Text = "";
			repoJSONTextEditorID = "";

			REPOJSONTextEditor.Visibility = Visibility.Visible;
			REPOVisualEditorTabControl.Visibility = Visibility.Collapsed;

			//int response = reset_repos();

			UInt32 isREPOLoaded = is_repo_loaded();

			UInt32 isORESLoaded = is_ores_loaded();

			if (isREPOLoaded == 0)
			{
				MessageBoxShow("No REPO hash file/resource found, ideally load all RPKGs from your Hitman Runtime folder and search again.");

				return;
			}

			if (isORESLoaded == 0)
			{
				MessageBoxShow("No ORES hash file/resource found, ideally load all RPKGs from your Hitman Runtime folder and search again.");

				return;
			}

			int response = load_repo();

			UInt32 repo_data_size = get_repo_response_data_size();

			byte[] repo_data = new byte[repo_data_size];

			Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

			if (repo_data_size > 0)
			{
				MessageBoxShow(Encoding.UTF8.GetString(repo_data));
			}
			else
			{
				LoadREPO();
			}
		}

		private void LoadREPOFromFileButton_Click(object sender, RoutedEventArgs e)
		{
			REPOJSONTextEditor.Text = "";
			repoJSONTextEditorID = "";

			REPOJSONTextEditor.Visibility = Visibility.Visible;
			REPOVisualEditorTabControl.Visibility = Visibility.Collapsed;

			string repoPath = SelectFile("input", "Select REPO File To Input", "All files|*.*", "");

			if (repoPath != "")
			{
				//int response = reset_repos();

				int response = load_repo_from_file(repoPath);

				UInt32 repo_data_size = get_repo_response_data_size();

				byte[] repo_data = new byte[repo_data_size];

				Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

				if (repo_data_size > 0)
				{
					MessageBoxShow(Encoding.UTF8.GetString(repo_data));
				}
				else
				{
					LoadREPO();
				}
			}
		}

		private void REPOImportSMFREPOJSONButton_Click(object sender, RoutedEventArgs e)
		{
			REPOJSONTextEditor.Text = "";
			repoJSONTextEditorID = "";

			REPOJSONTextEditor.Visibility = Visibility.Visible;
			REPOVisualEditorTabControl.Visibility = Visibility.Collapsed;

			if (REPOTreeView.Nodes.Count > 2)
			{
				string repoPath = SelectFile("input", "Select SMF REPO JSON File To Input", "SMF REPO JSON files|*.repository.json|All files|*.*", "");

				if (repoPath != "")
				{
					int response = import_patch(repoPath);

					UInt32 repo_data_size = get_repo_response_data_size();

					byte[] repo_data = new byte[repo_data_size];

					Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

					if (repo_data_size > 0)
					{
						MessageBoxShow(Encoding.UTF8.GetString(repo_data));
					}
					else
					{
						LoadREPO();
					}
				}
			}
			else
			{
				MessageBoxShow("Load the REPO either from the currently loaded RPKGs or from a REPO file first.");
			}
		}

		private void REPOSaveChangesToSMFREPOJSONButton_Click(object sender, RoutedEventArgs e)
		{
			if (REPOTreeView.Nodes.Count > 2)
			{
				string repoPath = SelectFile("output", "Select SMF REPO JSON File To Output To", "SMF REPO JSON files|*.repository.json|All files|*.*", "");

				if (repoPath != "")
				{
					if (!repoPath.ToLower().EndsWith(".repository.json"))
					{
						repoPath += ".repository.json";
					}

					int response = create_patch(repoPath);

					UInt32 repo_data_size = get_repo_response_data_size();

					byte[] repo_data = new byte[repo_data_size];

					Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

					if (repo_data_size > 0)
					{
						MessageBoxShow(Encoding.UTF8.GetString(repo_data));
					}
				}
			}
			else
			{
				MessageBoxShow("Load the REPO either from the currently loaded RPKGs or from a REPO file first.");
			}
		}

		private void REPOJSONTextEditor_TextChanged(object sender, EventArgs e)
		{
			//int isValidJSON = is_valid_json(REPOJSONTextEditor.Text);

			if (REPOJSONTextEditor.Text == "")
			{
				REPOJSONStatus.Visibility = Visibility.Collapsed;
			}
			else
			{
				string response = Marshal.PtrToStringAnsi(check_json(REPOJSONTextEditor.Text));

				//if (isValidJSON == 1)
				if (response == "")
				{
					//MessageBoxShow("Valid!");

					REPOJSONStatus.Visibility = Visibility.Visible;
					REPOJSONStatus.Foreground = System.Windows.Media.Brushes.PaleGreen;
					REPOJSONStatus.Content = "JSON: Valid";
				}
				else
				{
					//MessageBoxShow("Not Valid!");

					REPOJSONStatus.Visibility = Visibility.Visible;
					REPOJSONStatus.Foreground = System.Windows.Media.Brushes.OrangeRed;
					REPOJSONStatus.Content = "JSON: " + response;
				}
			}
		}

		private void REPOJSONTextEditor_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
		{
			//MessageBoxShow(REPOJSONTextEditorName.Text);

			if (repoJSONTextEditorID != "")
			{
				int response = save_json(repoJSONTextEditorID, REPOJSONTextEditor.Text);

				repoJSONTextEditorID = Marshal.PtrToStringAnsi(get_repo_response_data());

				if (repoJSONTextEditorID.StartsWith("Err"))
				{
					MessageBoxShow(repoJSONTextEditorID);

					REPOJSONTextEditor.Text = "";
					repoJSONTextEditorID = "";
				}
				else
				{
					UpdateNode();
				}
			}
		}

		void UpdateNode()
		{
			int response = get_repo_entry(repoJSONTextEditorID);

			UInt32 repo_data_size = get_repo_response_data_size();

			byte[] repo_data = new byte[repo_data_size];

			Marshal.Copy(get_repo_response_data(), repo_data, 0, (int)repo_data_size);

			if (repo_data_size > 0)
			{
				string[] entries = Encoding.UTF8.GetString(repo_data).Split(new[] { "||||||" }, StringSplitOptions.None);

				foreach (string entry in entries)
				{
					if (entry.Length > 0)
					{
						string[] data = entry.Split(new[] { "||||" }, StringSplitOptions.None);

						if (data.Length == 3)
						{
							System.Windows.Forms.TreeNode node = REPOTreeView.Nodes[repoJSONTextEditorParentNodeIndex].Nodes[repoJSONTextEditorNodeIndex];

							node.Text = data[0] + " (" + data[2] + ")";
							node.Name = data[2];
							node.Tag = data[1];
						}
					}
				}
			}
		}

		private void SearchREPOTextBox_TextChanged(object sender, TextChangedEventArgs e)
		{
			if (searchREPOInputTimer == null)
			{
				searchREPOInputTimer = new System.Windows.Threading.DispatcherTimer();

				searchREPOInputTimer.Interval = TimeSpan.FromMilliseconds(600);

				searchREPOInputTimer.Tick += SearchREPOTextBox_TimerTimeout;
			}

			searchREPOInputTimer.Stop();
			//searchREPOInputTimer.Tag = (sender as TextBox).Text;
			searchREPOInputTimer.Start();
		}

		private void SearchREPOTextBox_TimerTimeout(object sender, EventArgs e)
		{
			var timer = (sender as System.Windows.Threading.DispatcherTimer);

			if (timer == null)
			{
				return;
			}

			timer.Stop();

			REPOTreeView.BeginUpdate();

			repoTreeViewBackup.Restore();

			if (SearchREPOTextBox.Text.Length > 0)
			{
				string filter = SearchREPOTextBox.Text.ToLower();
				int childrenVisibleCount = 0;

				foreach (System.Windows.Forms.TreeNode node in REPOTreeView.Nodes)
				{
					FilterNodes(node, ref filter, childrenVisibleCount);
				}
			}

			REPOTreeView.EndUpdate();
		}

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

		private void REPOJSONEditorButton_Click(object sender, RoutedEventArgs e)
		{
			REPOJSONTextEditor.Visibility = Visibility.Visible;
			REPOVisualEditorTabControl.Visibility = Visibility.Collapsed;
		}

		private void REPOJSONVisualEditorButton_Click(object sender, RoutedEventArgs e)
		{
			REPOJSONTextEditor.Visibility = Visibility.Collapsed;
			REPOVisualEditorTabControl.Visibility = Visibility.Visible;

			LoadREPOVisualEditor();
		}

		Grid CreateInput(string labelText, int labelFontSize, FontWeight labelFontWeight, string textBoxName, string textBoxText, Thickness textBoxMargin, TextChangedEventHandler textChangedEventHandler)
		{
			Grid grid = new Grid();
			RowDefinition rowDefinition = new RowDefinition();
			ColumnDefinition columnDefinition = new ColumnDefinition();
			rowDefinition.Height = GridLength.Auto;
			grid.RowDefinitions.Add(rowDefinition);
			columnDefinition.Width = GridLength.Auto;
			grid.ColumnDefinitions.Add(columnDefinition);
			columnDefinition = new ColumnDefinition();
			columnDefinition.Width = new GridLength(1, GridUnitType.Star);
			grid.ColumnDefinitions.Add(columnDefinition);
			
			return grid;
		}

		TextBox CreateTextBox(string name, string text, Thickness margin, KeyboardFocusChangedEventHandler keyboardFocusChangedEventHandler)
		{
			TextBox textBox = new TextBox();
			textBox.Name = name;
			textBox.Text = text;
			textBox.Margin = margin;

			if (keyboardFocusChangedEventHandler != null)
			{
				textBox.LostKeyboardFocus += keyboardFocusChangedEventHandler;
			}

			return textBox;
		}

		TextBlock CreateTextBlock(string text, int fontsize, FontWeight fontWeight)
		{
			TextBlock label1 = new TextBlock();
			label1.Text = text;
			label1.FontSize = fontsize;
			label1.FontWeight = fontWeight;
			return label1;
		}

		void LoadREPOVisualEditor()
		{
			loadingVisualEditor = true;

			REPOVisualEditorTabControl.Items.Clear();

			TabItem tabItemRoot = new TabItem();
			tabItemRoot.Header = "Root";
			ScrollViewer scrollViewerRoot = new ScrollViewer();
			scrollViewerRoot.Name = tabItemRoot.Header + "ScrollViewer";
			scrollViewerRoot.VerticalScrollBarVisibility = ScrollBarVisibility.Auto;
			StackPanel stackPanelRoot = new StackPanel();
			stackPanelRoot.Name = tabItemRoot.Header + "StackPanel";
			scrollViewerRoot.Content = stackPanelRoot;
			tabItemRoot.Content = scrollViewerRoot;

			REPOVisualEditorTabControl.Items.Add(tabItemRoot);

			Grid gridRoot = new Grid();
			ColumnDefinition columnDefinition = new ColumnDefinition();
			columnDefinition.Width = new GridLength(1, GridUnitType.Star);
			gridRoot.ColumnDefinitions.Add(columnDefinition);
			columnDefinition = new ColumnDefinition();
			columnDefinition.Width = new GridLength(10);
			gridRoot.ColumnDefinitions.Add(columnDefinition);
			columnDefinition = new ColumnDefinition();
			columnDefinition.Width = new GridLength(10);
			gridRoot.ColumnDefinitions.Add(columnDefinition);
			columnDefinition = new ColumnDefinition();
			columnDefinition.Width = new GridLength(1, GridUnitType.Star);
			gridRoot.ColumnDefinitions.Add(columnDefinition);
			bool top = true;
			int index = 0;

			visualEditorJSON = JsonDocument.Parse(REPOJSONTextEditor.Text);

			foreach (JsonProperty jsonProperty in visualEditorJSON.RootElement.EnumerateObject())
			{
				string jsonPointer = "";

				if (jsonProperty.Value.ValueKind == JsonValueKind.Object)
				{
					TabItem tabItem = new TabItem();
					tabItem.Header = jsonProperty.Name;
					ScrollViewer scrollViewer = new ScrollViewer();
					scrollViewer.Name = tabItem.Header + "ScrollViewer";
					StackPanel stackPanel = new StackPanel();
					stackPanel.Name = tabItem.Header + "StackPanel";
					scrollViewer.Content = stackPanel;
					tabItem.Content = scrollViewer;
					REPOVisualEditorTabControl.Items.Add(tabItem);
					Grid gridObject = new Grid();
					ColumnDefinition columnDefinitionObject = new ColumnDefinition();
					columnDefinitionObject.Width = new GridLength(1, GridUnitType.Star);
					gridObject.ColumnDefinitions.Add(columnDefinitionObject);
					columnDefinitionObject = new ColumnDefinition();
					columnDefinitionObject.Width = new GridLength(10);
					gridObject.ColumnDefinitions.Add(columnDefinitionObject);
					columnDefinitionObject = new ColumnDefinition();
					columnDefinitionObject.Width = new GridLength(10);
					gridObject.ColumnDefinitions.Add(columnDefinitionObject);
					columnDefinitionObject = new ColumnDefinition();
					columnDefinitionObject.Width = new GridLength(1, GridUnitType.Star);
					gridObject.ColumnDefinitions.Add(columnDefinitionObject);
					bool objectTop = true;
					int objectIndex = 0;

					jsonPointer = "/" + jsonProperty.Name.Replace("~", "~0").Replace("/", "~1");

					foreach (JsonProperty jsonProperty2 in jsonProperty.Value.EnumerateObject())
					{
						if (jsonProperty2.Value.ValueKind == JsonValueKind.Array)
						{
							int arrayIndex = 0;

							jsonPointer = "/" + jsonProperty2.Name.Replace("~", "~0").Replace("/", "~1");

							foreach (JsonElement jsonElement in jsonProperty2.Value.EnumerateArray())
							{
								string arrayJsonPointer = jsonPointer + "/" + arrayIndex.ToString();

								AddRowToVisualEditor(jsonProperty2.Name + " (" + arrayIndex.ToString() + ")", jsonElement, ref arrayJsonPointer, ref gridObject, ref objectIndex, ref objectTop);

								arrayIndex++;
							}
						}
						else
						{
							string valueJsonPointer = jsonPointer + "/" + jsonProperty2.Name.Replace("~", "~0").Replace("/", "~1");

							AddRowToVisualEditor(jsonProperty2.Name, jsonProperty2.Value, ref valueJsonPointer, ref gridObject, ref objectIndex, ref objectTop);
						}
					}

					stackPanel.Children.Add(gridObject);
				}
				else if (jsonProperty.Value.ValueKind == JsonValueKind.Array)
				{
					int arrayIndex = 0;

					jsonPointer = "/" + jsonProperty.Name.Replace("~", "~0").Replace("/", "~1");

					foreach (JsonElement jsonElement in jsonProperty.Value.EnumerateArray())
					{
						string arrayJsonPointer = jsonPointer + "/" + arrayIndex.ToString();

						AddRowToVisualEditor(jsonProperty.Name + " (" + arrayIndex.ToString() + ")", jsonElement, ref arrayJsonPointer, ref gridRoot, ref index, ref top);

						arrayIndex++;
					}
				}
				else
				{
					jsonPointer = "/" + jsonProperty.Name.Replace("~", "~0").Replace("/", "~1");

					AddRowToVisualEditor(jsonProperty.Name, jsonProperty.Value, ref jsonPointer, ref gridRoot, ref index, ref top);
				}
			}

			stackPanelRoot.Children.Add(gridRoot);

			REPOVisualEditorTabControl.SelectedIndex = 0;

			loadingVisualEditor = false;
		}

		void AddRowToVisualEditor(string name, JsonElement element, ref string jsonPointer, ref Grid grid, ref int index, ref bool top)
		{
			RowDefinition rowDefinition = new RowDefinition();
			rowDefinition.Height = GridLength.Auto;
			grid.RowDefinitions.Add(rowDefinition);
			TextBlock textBlock = CreateTextBlock(name + ":", 14, FontWeights.Regular);
			textBlock.TextAlignment = TextAlignment.Right;
			textBlock.FontWeight = FontWeights.Bold;
			textBlock.HorizontalAlignment = HorizontalAlignment.Stretch;
			textBlock.VerticalAlignment = VerticalAlignment.Center;
			System.Windows.Media.BrushConverter brushConverter = new BrushConverter();
			textBlock.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#2e2e2e");
			Border border = new Border();
			border.BorderBrush = System.Windows.Media.Brushes.White;

			if (top)
				border.BorderThickness = new Thickness(1, 1, 0, 1);
			else
				border.BorderThickness = new Thickness(1, 0, 0, 1);

			border.Margin = new Thickness(50, 0, 0, 0);
			border.Child = textBlock;
			grid.Children.Add(border);
			Grid.SetRow(border, index);
			Grid.SetColumn(border, 0);
			border = new Border();
			border.BorderBrush = System.Windows.Media.Brushes.White;

			if (top)
				border.BorderThickness = new Thickness(0, 1, 0, 1);
			else
				border.BorderThickness = new Thickness(0, 0, 0, 1);

			grid.Children.Add(border);
			Grid.SetRow(border, index);
			Grid.SetColumn(border, 1);
			border = new Border();
			border.BorderBrush = System.Windows.Media.Brushes.White;
			border.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");

			if (top)
				border.BorderThickness = new Thickness(1, 1, 0, 1);
			else
				border.BorderThickness = new Thickness(1, 0, 0, 1);

			grid.Children.Add(border);
			Grid.SetRow(border, index);
			Grid.SetColumn(border, 2);

			if (enumDictionary.ContainsKey(name))
			{
				ComboBox comboBox = new ComboBox();
				comboBox.ItemsSource = enumDictionary[name];
				comboBox.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");
				comboBox.BorderThickness = new Thickness(0);
				comboBox.Tag = jsonPointer;
				comboBox.SelectionChanged += ComboBox_SelectionChanged;

				if (!enumDictionary[name].Contains<string>(element.ToString()))
				{
					comboBox.Items.Add(element.ToString());
				}

				comboBox.SelectedValue = element.ToString();
				border = new Border();
				border.BorderBrush = System.Windows.Media.Brushes.White;
				border.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");

				if (top)
					border.BorderThickness = new Thickness(0, 1, 1, 1);
				else
					border.BorderThickness = new Thickness(0, 0, 1, 1);

				border.Margin = new Thickness(0, 0, 50, 0);
				border.Child = comboBox;
				grid.Children.Add(border);
				Grid.SetRow(border, index);
				Grid.SetColumn(border, 3);
			}
			else if (element.ValueKind == JsonValueKind.True || element.ValueKind == JsonValueKind.False)
			{
				CheckBox checkBox = new CheckBox();
				checkBox.Content = name;
				checkBox.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");
				checkBox.BorderThickness = new Thickness(0);
				checkBox.Tag = jsonPointer;
				checkBox.Checked += CheckBox_Changed;
				checkBox.Unchecked += CheckBox_Changed;

				if (element.ValueKind == JsonValueKind.True)
					checkBox.IsChecked = true;
				else
					checkBox.IsChecked = false;

				border = new Border();
				border.Height = 28;
				border.BorderBrush = System.Windows.Media.Brushes.White;
				border.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");

				if (top)
					border.BorderThickness = new Thickness(0, 1, 1, 1);
				else
					border.BorderThickness = new Thickness(0, 0, 1, 1);

				border.Margin = new Thickness(0, 0, 50, 0);
				border.Child = checkBox;
				grid.Children.Add(border);
				Grid.SetRow(border, index);
				Grid.SetColumn(border, 3);
			}
			else
			{
				TextBox textBox = CreateTextBox("testing", element.ToString(), new Thickness(0), TextBox_LostKeyboardFocus);
				textBox.TextAlignment = TextAlignment.Left;
				textBox.FontWeight = FontWeights.Regular;
				textBox.HorizontalAlignment = HorizontalAlignment.Stretch;
				textBox.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");
				textBox.BorderThickness = new Thickness(0);
				textBox.Tag = jsonPointer;
				border = new Border();
				border.BorderBrush = System.Windows.Media.Brushes.White;
				border.Background = (System.Windows.Media.Brush)brushConverter.ConvertFromString("#434343");

				if (top)
					border.BorderThickness = new Thickness(0, 1, 1, 1);
				else
					border.BorderThickness = new Thickness(0, 0, 1, 1);

				border.Margin = new Thickness(0, 0, 50, 0);
				border.Child = textBox;
				grid.Children.Add(border);
				Grid.SetRow(border, index);
				Grid.SetColumn(border, 3);
			}

			if (top)
				top = false;
			index++;
		}

		private static DependencyObject FindDescendant(DependencyObject parent, string name)
		{
			FrameworkElement element = parent as FrameworkElement;

			if (element != null)
			{
				if (element.Name == name)
				{
					return parent;
				}
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

		private void TextBox_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
		{
			if (loadingVisualEditor)
				return;

			TextBox textBox = sender as TextBox;

			string pointer = (textBox.Tag as string);

			if (pointer != null)
			{
				//MessageBoxShow(repoJSONTextEditorID + ", " + pointer + ", " + textBox.Text);

				update_json_at_pointer(repoJSONTextEditorID, pointer, textBox.Text);

				repoJSONTextEditorID = Marshal.PtrToStringAnsi(get_repo_response_data());

				if (repoJSONTextEditorID.StartsWith("Err"))
				{
					MessageBoxShow(repoJSONTextEditorID);

					REPOJSONTextEditor.Text = "";
					repoJSONTextEditorID = "";
				}
				else
				{
					System.Windows.Forms.TreeNode node = REPOTreeView.Nodes[repoJSONTextEditorParentNodeIndex].Nodes[repoJSONTextEditorNodeIndex];

					LoadREPOJSON(ref node);

					UpdateNode();
				}
			}
		}

		string GetJSONValueByPointer(string pointer)
		{
			foreach (JsonProperty jsonProperty in visualEditorJSON.RootElement.EnumerateObject())
			{
				string jsonPointer = "";

				if (jsonProperty.Value.ValueKind == JsonValueKind.Object)
				{
					jsonPointer = "/" + jsonProperty.Name.Replace("~", "~0").Replace("/", "~1");

					foreach (JsonProperty jsonProperty2 in jsonProperty.Value.EnumerateObject())
					{
						if (jsonProperty2.Value.ValueKind == JsonValueKind.Array)
						{
							int arrayIndex = 0;

							jsonPointer = "/" + jsonProperty2.Name.Replace("~", "~0").Replace("/", "~1");

							foreach (JsonElement jsonElement in jsonProperty2.Value.EnumerateArray())
							{
								string arrayJsonPointer = jsonPointer + "/" + arrayIndex.ToString();

								if (pointer == arrayJsonPointer)
								{
									return jsonElement.ToString();
								}

								arrayIndex++;
							}
						}
						else
						{
							string valueJsonPointer = jsonPointer + "/" + jsonProperty2.Name.Replace("~", "~0").Replace("/", "~1");

							if (pointer == valueJsonPointer)
							{
								return jsonProperty2.Value.ToString();
							}

						}
					}
				}
				else if (jsonProperty.Value.ValueKind == JsonValueKind.Array)
				{
					int arrayIndex = 0;

					jsonPointer = "/" + jsonProperty.Name.Replace("~", "~0").Replace("/", "~1");

					foreach (JsonElement jsonElement in jsonProperty.Value.EnumerateArray())
					{
						string arrayJsonPointer = jsonPointer + "/" + arrayIndex.ToString();

						if (pointer == arrayJsonPointer)
						{
							return jsonElement.ToString();
						}

						arrayIndex++;
					}
				}
				else
				{
					jsonPointer = "/" + jsonProperty.Name.Replace("~", "~0").Replace("/", "~1");

					if (pointer == jsonPointer)
					{
						return jsonProperty.Value.ToString();
					}
				}
			}

			return "";
		}

		private void CheckBox_Changed(object sender, RoutedEventArgs e)
		{
			if (loadingVisualEditor)
				return;

			CheckBox checkBox = sender as CheckBox;

			string pointer = (checkBox.Tag as string);

			if (pointer != null)
			{
				if (checkBox.IsChecked ?? false)
				{
					update_json_at_pointer(repoJSONTextEditorID, pointer, "true");
				}
				else
				{
					update_json_at_pointer(repoJSONTextEditorID, pointer, "false");
				}

				System.Windows.Forms.TreeNode node = REPOTreeView.Nodes[repoJSONTextEditorParentNodeIndex].Nodes[repoJSONTextEditorNodeIndex];

				LoadREPOJSON(ref node);

				UpdateNode();
			}
		}

		private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			if (loadingVisualEditor)
				return;

			ComboBox comboBox = sender as ComboBox;

			string comboBoxText = comboBox.SelectedItem.ToString();

			string pointer = (comboBox.Tag as string);

			if (pointer != null)
			{
				if (comboBoxText != GetJSONValueByPointer(pointer))
				{
					//MessageBoxShow(repoJSONTextEditorID + ", " + pointer + ", " + comboBoxText);

					update_json_at_pointer(repoJSONTextEditorID, pointer, comboBoxText);

					System.Windows.Forms.TreeNode node = REPOTreeView.Nodes[repoJSONTextEditorParentNodeIndex].Nodes[repoJSONTextEditorNodeIndex];

					LoadREPOJSON(ref node);

					UpdateNode();
				}
			}
		}

		private void HexViewerTextBox_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
		{
			bool controlKey = Keyboard.Modifiers == ModifierKeys.Control;

			if (controlKey)
			{
				if (e.Delta > 0)
				{
					if (HexViewerTextBox.FontSize < 64)
					{
						HexViewerTextBox.FontSize = Math.Min(64, HexViewerTextBox.FontSize + 1);
					}
				}
				else
				{
					if (HexViewerTextBox.FontSize > 4)
					{
						HexViewerTextBox.FontSize = Math.Max(4, HexViewerTextBox.FontSize - 1);
					}
				}
			}
		}

		private void DetailsTextBox_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
		{
			bool controlKey = Keyboard.Modifiers == ModifierKeys.Control;

			if (controlKey)
			{
				if (e.Delta > 0)
				{
					if (DetailsTextBox.FontSize < 64)
					{
						DetailsTextBox.FontSize = Math.Min(64, DetailsTextBox.FontSize + 1);
					}
				}
				else
				{
					if (DetailsTextBox.FontSize > 4)
					{
						DetailsTextBox.FontSize = Math.Max(4, DetailsTextBox.FontSize - 1);
					}
				}
			}
		}

		private void LocalizationTextBox_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
		{
			bool controlKey = Keyboard.Modifiers == ModifierKeys.Control;

			if (controlKey)
			{
				if (e.Delta > 0)
				{
					if (LocalizationTextBox.FontSize < 64)
					{
						LocalizationTextBox.FontSize = Math.Min(64, LocalizationTextBox.FontSize + 1);
					}
				}
				else
				{
					if (LocalizationTextBox.FontSize > 4)
					{
						LocalizationTextBox.FontSize = Math.Max(4, LocalizationTextBox.FontSize - 1);
					}
				}
			}
		}

		private void REPOHashTextBox_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
		{
			bool controlKey = Keyboard.Modifiers == ModifierKeys.Control;

			if (controlKey)
			{
				if (e.Delta > 0)
				{
					if (REPOHashTextBox.FontSize < 64)
					{
						REPOHashTextBox.FontSize = Math.Min(64, REPOHashTextBox.FontSize + 1);
					}
				}
				else
				{
					if (REPOHashTextBox.FontSize > 4)
					{
						REPOHashTextBox.FontSize = Math.Max(4, REPOHashTextBox.FontSize - 1);
					}
				}
			}
		}

		private void REPOJSONTextEditor_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
		{

			bool controlKey = Keyboard.Modifiers == ModifierKeys.Control;

			if (controlKey)
			{
				if (e.Delta > 0)
				{
					if (REPOJSONTextEditor.FontSize < 64)
					{
						REPOJSONTextEditor.FontSize = Math.Min(64, REPOJSONTextEditor.FontSize + 1);
					}
				}
				else
				{
					if (REPOJSONTextEditor.FontSize > 4)
					{
						REPOJSONTextEditor.FontSize = Math.Max(4, REPOJSONTextEditor.FontSize - 1);
					}
				}
			}
		}

		private void HandleTreeViewMouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			System.Windows.Forms.TreeView treeView = (sender as System.Windows.Forms.TreeView);

			bool controlKey = Keyboard.Modifiers == ModifierKeys.Control;

			if (controlKey)
			{
				if (e.Delta > 0)
				{
					if (treeView.Font.Size < 64)
					{
						treeView.Font = new System.Drawing.Font("Consolas", Math.Min(64, treeView.Font.Size + 1));
					}
				}
				else
				{
					if (treeView.Font.Size > 4)
					{
						treeView.Font = new System.Drawing.Font("Consolas", Math.Max(4, treeView.Font.Size - 1));
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------//
// MiniKey data node text source
//   - file path  : PackRoot.msd
//   - time stamp : 2015.06.19 (18:27:37)
//   - exporter   : client (Test)
//--------------------------------------------------------------------//


PushTemplate "MkSceneNode";

Node "[SUBNODES]"
{
	Node "Main" : "MkSceneNode"
	{
		Node "[PANELS]"
		{
			Node "ImgTest" : "MkPanel"
			{
				uint Attribute = 26;
				str ImagePath = "Image\s01.jpg";
				float LocalDepth = 1002.000;
				vec2 PanelSize = (450.000, 250.000);
			}

			Node "TextTest" : "MkPanel"
			{
				uint Attribute = 26;
				float LocalDepth = 1001.000;
				vec2 PanelSize = (110.000, 250.000);
				bool TextNodeWR = Yes;

				Node "TextNodeData"
				{
					int LFV = 10;
					str Seq = // [2]
						"1st" /
						"2st";
					str Style = "Desc:Title";
					str Type = "LargeBold";

					Node "1st"
					{
						str Text = "[ ù��° �����Դϴ� ]";

						Node "Sub list"
						{
							int LFH = 8;
							int LFV = 3;
							str Seq = // [2]
								"�Ϲ��̰�" /
								"�̹��̱���";
							str Style = "Desc:Normal";
							str Type = "Medium";

							Node "�̹��̱���"
							{
								str Text = "- ��� �׸� �ι�°�ε�<LF>- �ڵ� ������ ������ ���";
							}

							Node "�Ϲ��̰�"
							{
								str Text = "<LF>- ��� �׸� ù��°�̰�<LF>";
							}
						}
					}

					Node "2st"
					{
						str Text = "<LF>[ �̰��� �ι�° ���� ]";

						Node "0"
						{
							int LFV = 3;
							str Seq = // [2]
								"0" /
								"1";
							str Style = "Desc:Normal";
							str Type = "Medium";

							Node "0"
							{
								str Text = "<LF>  - ���� �׸� ù��°";
							}

							Node "1"
							{
								str Seq = // [3]
									"100" /
									"200" /
									"300";

								Node "100"
								{
									str Text = "<LF>  - ���� �׸� �ι�°�ε� ";
								}

								Node "200"
								{
									str Style = "Desc:Highlight";
									str Text = "<% �ٲ��ֽÿ� %>";
									str Type = "Special";
								}

								Node "300"
								{
									str Text = " <- �ٲٰ� ����   <LF>  - ����<LF>";
								}
							}
						}
					}
				}
			}
		}

		Node "[SUBNODES]"
		{
			Node "BG" : "MkSceneNode"
			{
				float LocalDepth = 10000.000;

				Node "[PANELS]"
				{
					Node "P" : "MkPanel"
					{
						str ImagePath = "Image\rohan_screenshot.png";
						vec2 PanelSize = (1024.000, 768.000);
					}
				}
			}
		}
	}

	Node "WinMgr" : "MkWindowManagerNode"
	{
		str ActivateWins = // [2]
			"TitleBarC" /
			"TitleBar";
		str Modal = "TitleBarB";

		Node "[SUBNODES]"
		{
			Node "TitleBar" : "MkTitleBarControlNode"
			{
				uint Attribute = 59;
				int CaptionAlignPos = 50;
				str CaptionString = "�ڷ��� Ÿ��Ʋ����!!!";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (344.000, 12.000);
				str ComponentType = "eCT_Title";
				int IconType = 1;
				float LocalDepth = 600.000;
				vec2 LocalPosition = (400.000, 600.000);
				str ThemeName = "Default";
				str WinFrameType = "eFT_Small";

				Node "[SUBNODES]"
				{
					Node "BodyFrame" : "MkBodyFrameControlNode"
					{
						vec2 AlignOffset = (-3.000, 3.000);
						int AlignPosition = 34;
						uint Attribute = 31;
						vec2 ClientSize = (350.000, 250.000);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						vec2 LocalPosition = (-3.000, -235.000);
						str ThemeName = "Default";
						bool UseShadow = Yes;

						Node "[SUBNODES]"
						{
							Node "BodyClient" : "MkWindowThemedNode"
							{
								vec2 AlignOffset = (0.000, 10.000);
								int AlignPosition = 67;
								vec2 ClientSize = (330.000, 205.000);
								str ComponentType = "eCT_StaticBox";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (10.000, 13.000);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "ScenePortal" : "MkScenePortalNode"
									{
										vec2 AlignOffset = (50.000, 10.000);
										int AlignPosition = 66;
										vec2 ClientSize = (250.000, 180.000);
										float LocalDepth = -1.000;
										vec2 LocalPosition = (53.000, 13.000);
										str ThemeName = "Default";

										Node "DestWindowMgr" : "MkWindowManagerNode"
										{
											str ActivateWins = "STitleBar";

											Node "[SUBNODES]"
											{
												Node "STitleBar" : "MkTitleBarControlNode"
												{
													uint Attribute = 59;
													int CaptionAlignPos = 50;
													str CaptionString = "���� ������";
													str CaptionTextName = // [3]
														"Theme" /
														"Default" /
														"Small";
													vec2 ClientSize = (244.000, 12.000);
													str ComponentType = "eCT_Title";
													int IconType = 1;
													float LocalDepth = 200.000;
													vec2 LocalPosition = (0.000, 162.000);
													str ThemeName = "Default";
													str WinFrameType = "eFT_Small";

													Node "[SUBNODES]"
													{
														Node "SBodyFrame" : "MkBodyFrameControlNode"
														{
															int AlignPosition = 82;
															uint Attribute = 31;
															vec2 ClientSize = (244.000, 300.000);
															str ComponentType = "eCT_NoticeBox";
															int HangingType = 2;
															float LocalDepth = 0.100;
															vec2 LocalPosition = (0.000, -306.000);
															str ThemeName = "Default";

															Node "[SUBNODES]"
															{
																Node "EB" : "MkEditBoxControlNode"
																{
																	vec2 AlignOffset = (10.000, -50.000);
																	int AlignPosition = 34;
																	vec2 ClientSize = (210.000, 12.000);
																	str ComponentType = "eCT_DefaultBox";
																	bool History = Yes;
																	float LocalDepth = -1.000;
																	vec2 LocalPosition = (13.000, 235.000);
																	str TextString = "�Ʊ� �ٸ�";
																	str ThemeName = "Default";
																	str WinFrameType = "eFT_Small";
																}

																Node "Slider" : "MkSliderControlNode"
																{
																	vec2 AlignOffset = (10.000, -100.000);
																	int AlignPosition = 34;
																	int BeginValue = 10;
																	vec2 ClientSize = (210.000, 0.000);
																	str ComponentType = "eCT_GuideLine";
																	int CurrentValue = 20;
																	bool Horizontal = Yes;
																	float LineLength = 210.000;
																	float LocalDepth = -1.000;
																	vec2 LocalPosition = (13.000, 197.000);
																	bool ShowValue = Yes;
																	str ThemeName = "Default";
																	int ValueSize = 90;
																	str WinFrameType = "eFT_Small";
																}

																Node "SubCCBtn" : "MkWindowBaseNode"
																{
																	vec2 AlignOffset = (120.000, -10.000);
																	int AlignPosition = 34;
																	vec2 ClientSize = (100.000, 20.000);
																	str ComponentType = "eCT_CancelBtn";
																	float LocalDepth = -1.000;
																	vec2 LocalPosition = (123.000, 267.000);
																	str ThemeName = "Default";
																}

																Node "SubOKBtn" : "MkWindowBaseNode"
																{
																	vec2 AlignOffset = (10.000, -10.000);
																	int AlignPosition = 34;
																	vec2 ClientSize = (100.000, 20.000);
																	str ComponentType = "eCT_OKBtn";
																	float LocalDepth = -1.000;
																	vec2 LocalPosition = (13.000, 267.000);
																	str ThemeName = "Default";
																}
															}
														}
													}
												}
											}
										}
									}

									Node "Slider" : "MkSliderControlNode"
									{
										vec2 AlignOffset = (10.000, -30.000);
										int AlignPosition = 34;
										int BeginValue = 5;
										vec2 ClientSize = (0.000, 150.000);
										str ComponentType = "eCT_GuideLine";
										int CurrentValue = 12;
										float LineLength = 150.000;
										float LocalDepth = -1.000;
										vec2 LocalPosition = (13.000, 22.000);
										bool ShowValue = Yes;
										str ThemeName = "Default";
										int ValueSize = 15;
										str WinFrameType = "eFT_Small";
									}
								}
							}
						}
					}
				}
			}

			Node "TitleBarB" : "MkTitleBarControlNode"
			{
				uint Attribute = 59;
				int CaptionAlignPos = 50;
				str CaptionString = "�ڷ��� Ÿ��Ʋ����B";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (344.000, 12.000);
				str ComponentType = "eCT_Title";
				int IconType = 1;
				float LocalDepth = 100.000;
				vec2 LocalPosition = (200.000, 500.000);
				str ThemeName = "Default";
				str WinFrameType = "eFT_Small";

				Node "[SUBNODES]"
				{
					Node "BodyFrameB" : "MkBodyFrameControlNode"
					{
						vec2 AlignOffset = (-3.000, 3.000);
						int AlignPosition = 34;
						uint Attribute = 31;
						vec2 ClientSize = (350.000, 250.000);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						vec2 LocalPosition = (-3.000, -235.000);
						str ThemeName = "Default";
						bool UseShadow = Yes;

						Node "[SUBNODES]"
						{
							Node "ListBox" : "MkListBoxControlNode"
							{
								vec2 AlignOffset = (10.000, -30.000);
								int AlignPosition = 34;
								vec2 ClientSize = (206.000, 108.000);
								str ComponentType = "eCT_DefaultBox";
								str ItemKeyList = // [8]
									"aaa" /
									"������" /
									"bbb" /
									"ccc" /
									"������" /
									"��" /
									"����" /
									"��������";
								str ItemMsgList = // [8]
									"aaa" /
									"������" /
									"bbb" /
									"ccc" /
									"������" /
									"��" /
									"����" /
									"��������";
								str ItemSequence = // [8]
									"aaa" /
									"bbb" /
									"ccc" /
									"������" /
									"������" /
									"����" /
									"��������" /
									"��";
								float ItemWidth = 200.000;
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 109.000);
								int OnePageItemSize = 6;
								str ThemeName = "Default";
								str WinFrameType = "eFT_Small";
							}
						}
					}
				}
			}

			Node "TitleBarC" : "MkTitleBarControlNode"
			{
				uint Attribute = 59;
				int CaptionAlignPos = 50;
				str CaptionString = "�ڷ��� Ÿ��Ʋ����C";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (344.000, 12.000);
				str ComponentType = "eCT_Title";
				int IconType = 1;
				float LocalDepth = 200.000;
				vec2 LocalPosition = (100.000, 400.000);
				str ThemeName = "Default";
				str WinFrameType = "eFT_Small";

				Node "[SUBNODES]"
				{
					Node "BodyFrameC" : "MkBodyFrameControlNode"
					{
						vec2 AlignOffset = (-3.000, 3.000);
						int AlignPosition = 34;
						uint Attribute = 31;
						vec2 ClientSize = (350.000, 250.000);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						vec2 LocalPosition = (-3.000, -235.000);
						str ThemeName = "Default";
						bool UseShadow = Yes;

						Node "[SUBNODES]"
						{
							Node "CheckBox" : "MkCheckBoxControlNode"
							{
								vec2 AlignOffset = (10.000, -30.000);
								int AlignPosition = 34;
								str CaptionString = "üũ�ڽ� ����";
								str CaptionTextName = // [3]
									"Theme" /
									"Default" /
									"Small";
								vec2 ClientSize = (12.000, 12.000);
								str ComponentType = "eCT_CheckBoxBtn";
								vec2 LocalPosition = (13.000, 205.000);
								bool OnCheck = Yes;
								str ThemeName = "Default";
								str WinFrameType = "eFT_Small";
							}
						}
					}
				}
			}
		}
	}
}
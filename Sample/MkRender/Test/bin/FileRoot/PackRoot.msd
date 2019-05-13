//--------------------------------------------------------------------//
// MiniKey data node text source
//   - file path : PackRoot.msd
//   - time stamp : 2019.05.02 (18:07:57)
//   - exporter : pc (Test)
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
				uint Attribute = 18;
				str ImagePath = "Image\s01.jpg";
				float LocalDepth = 9999;
				vec2 PanelSize = (909, 505);
			}
		}

		Node "[SUBNODES]"
		{
			Node "BG" : "MkSceneNode"
			{
				float LocalDepth = 10000;

				Node "[PANELS]"
				{
					Node "P" : "MkPanel"
					{
						str ImagePath = "Image\rohan_screenshot.png";
						vec2 PanelSize = (1024, 768);

						Node "ShaderEffect"
						{
							str EffectName = "ttt";
							str ImagePath = // [2]
								"Image\s01.jpg" /
								"";
						}
					}
				}
			}
		}
	}

	Node "WinMgr" : "MkWindowManagerNode"
	{
		str ActivateWins = // [3]
			"Win(3)" /
			"Win(2)" /
			"Win(1)";
		str DeactivateWins = "MsgBox";

		Node "[SUBNODES]"
		{
			Node "MsgBox" : "MkTitleBarControlNode"
			{
				uint Attribute = 197890;
				int CaptionAlignPos = 50;
				str CaptionString = "캡션입니당";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (188, 12);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 3;
				vec2 LocalPosition = (500, 300);
				str ThemeName = "Default";
				bool UseCloseBtn = false;

				Node "[SUBNODES]"
				{
					Node "__#WC:BodyFrame" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (194, 104);
						str ComponentType = "eCT_NoticeBox";
						int HangingType = 3;
						float LocalDepth = 0.1;
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[SUBNODES]"
						{
							Node "__#WC:Cancel" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (100, 8);
								int AlignPosition = 66;
								vec2 ClientSize = (80, 12);
								str ComponentType = "eCT_CancelBtn";
								float LocalDepth = -1;
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.1;
										str TextMsg = "Cancel";
										str TextName = // [3]
											"Theme" /
											"Default" /
											"Small";
									}
								}
							}

							Node "__#WC:OAOTag" : "MkWindowTagNode"
							{
								vec2 AlignOffset = (32.5, 34);
								int AlignPosition = 66;
								float LocalDepth = -0.1;
								str TextMsg = "너무너무 잘생겨서<LF>어케 할지를 모르겠으용<LF>~(-_-)~";
								str TextName = // [3]
									"Theme" /
									"Default" /
									"Small";
							}

							Node "__#WC:Ok" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (8, 8);
								int AlignPosition = 66;
								vec2 ClientSize = (80, 12);
								str ComponentType = "eCT_OKBtn";
								float LocalDepth = -1;
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.1;
										str TextMsg = "OK";
										str TextName = // [3]
											"Theme" /
											"Default" /
											"Small";
									}
								}
							}
						}
					}
				}
			}

			Node "Win(1)" : "MkTitleBarControlNode"
			{
				uint Attribute = 197891;
				int CaptionAlignPos = 50;
				str CaptionString = "(1) 윈도우 안의 윈도우 + 에디트박스";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (344, 12);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 1;
				float LocalDepth = 733.33331;
				vec2 LocalPosition = (400, 600);
				str ThemeName = "Default";

				Node "[SUBNODES]"
				{
					Node "BodyFrame" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (350, 250);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.1;
						vec2 LocalPosition = (-3, -235);
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[SUBNODES]"
						{
							Node "BodyClient" : "MkWindowThemedNode"
							{
								vec2 AlignOffset = (0, 10);
								int AlignPosition = 67;
								vec2 ClientSize = (330, 205);
								str ComponentType = "eCT_StaticBox";
								float LocalDepth = -1;
								vec2 LocalPosition = (10, 13);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "ScenePortal" : "MkScenePortalNode"
									{
										vec2 AlignOffset = (50, 10);
										int AlignPosition = 66;
										vec2 ClientSize = (250, 180);
										float LocalDepth = -1;
										vec2 LocalPosition = (53, 13);
										str ThemeName = "Default";

										Node "DestWindowMgr" : "MkWindowManagerNode"
										{
											str ActivateWins = "STitleBar";

											Node "[SUBNODES]"
											{
												Node "STitleBar" : "MkTitleBarControlNode"
												{
													uint Attribute = 197891;
													int CaptionAlignPos = 50;
													str CaptionString = "서브 윈도우";
													str CaptionTextName = // [3]
														"Theme" /
														"Default" /
														"Small";
													vec2 ClientSize = (244, 12);
													str ComponentType = "eCT_Title";
													str FrameType = "eFT_Small";
													int IconType = 1;
													float LocalDepth = 200;
													vec2 LocalPosition = (0, 162);
													str ThemeName = "Default";

													Node "[SUBNODES]"
													{
														Node "SBodyFrame" : "MkBodyFrameControlNode"
														{
															int AlignPosition = 82;
															uint Attribute = 67329;
															vec2 ClientSize = (244, 300);
															str ComponentType = "eCT_NoticeBox";
															int HangingType = 2;
															float LocalDepth = 0.1;
															vec2 LocalPosition = (0, -306);
															str ThemeName = "Default";

															Node "[SUBNODES]"
															{
																Node "EB" : "MkEditBoxControlNode"
																{
																	vec2 AlignOffset = (10, -50);
																	int AlignPosition = 34;
																	vec2 ClientSize = (210, 12);
																	str ComponentType = "eCT_DefaultBox";
																	str FrameType = "eFT_Small";
																	bool History = true;
																	float LocalDepth = -1;
																	vec2 LocalPosition = (13, 235);
																	str TextString = "아기 다리";
																	str ThemeName = "Default";
																}

																Node "Slider" : "MkSliderControlNode"
																{
																	vec2 AlignOffset = (10, -100);
																	int AlignPosition = 34;
																	int BeginValue = 10;
																	vec2 ClientSize = (210, 0);
																	str ComponentType = "eCT_GuideLine";
																	int CurrentValue = 20;
																	str FrameType = "eFT_Small";
																	bool Horizontal = true;
																	float LineLength = 210;
																	float LocalDepth = -1;
																	vec2 LocalPosition = (13, 197);
																	bool ShowValue = true;
																	str ThemeName = "Default";
																	int ValueSize = 90;
																}

																Node "SubCCBtn" : "MkWindowBaseNode"
																{
																	vec2 AlignOffset = (120, -10);
																	int AlignPosition = 34;
																	vec2 ClientSize = (100, 20);
																	str ComponentType = "eCT_CancelBtn";
																	float LocalDepth = -1;
																	vec2 LocalPosition = (123, 267);
																	str ThemeName = "Default";
																}

																Node "SubOKBtn" : "MkWindowBaseNode"
																{
																	vec2 AlignOffset = (10, -10);
																	int AlignPosition = 34;
																	vec2 ClientSize = (100, 20);
																	str ComponentType = "eCT_OKBtn";
																	float LocalDepth = -1;
																	vec2 LocalPosition = (13, 267);
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
										vec2 AlignOffset = (10, -30);
										int AlignPosition = 34;
										vec2 ClientSize = (0, 150);
										str ComponentType = "eCT_GuideLine";
										int CurrentValue = 5;
										str FrameType = "eFT_Small";
										float LineLength = 150;
										float LocalDepth = -1;
										vec2 LocalPosition = (13, 22);
										bool ShowValue = true;
										str ThemeName = "Default";
										int ValueSize = 10;
									}
								}
							}
						}
					}
				}
			}

			Node "Win(2)" : "MkTitleBarControlNode"
			{
				uint Attribute = 197891;
				int CaptionAlignPos = 50;
				str CaptionString = "(2) 리스트 + 프로그레스 + client size 변경";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (354, 12);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 1;
				float LocalDepth = 466.66666;
				vec2 LocalPosition = (200, 500);
				str ThemeName = "Default";

				Node "[SUBNODES]"
				{
					Node "BodyFrameB" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (360, 250);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.1;
						vec2 LocalPosition = (-3, -235);
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[SUBNODES]"
						{
							Node "ListBox" : "MkListBoxControlNode"
							{
								vec2 AlignOffset = (10, -30);
								int AlignPosition = 34;
								vec2 ClientSize = (206, 108);
								str ComponentType = "eCT_DefaultBox";
								str FrameType = "eFT_Small";
								str ItemKeyList = // [8]
									"aaa" /
									"가나다" /
									"bbb" /
									"ccc" /
									"가고파" /
									"웁스" /
									"도나" /
									"도나도나";
								str ItemMsgList = // [8]
									"aaa" /
									"가나다" /
									"bbb" /
									"ccc" /
									"가고파" /
									"웁스" /
									"도나" /
									"도나도나";
								str ItemSequence = // [8]
									"aaa" /
									"bbb" /
									"ccc" /
									"가고파" /
									"가나다" /
									"도나" /
									"도나도나" /
									"웁스";
								float ItemWidth = 200;
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 109);
								int OnePageItemSize = 6;
								str ThemeName = "Default";
							}

							Node "ProgBar" : "MkProgressBarNode"
							{
								vec2 AlignOffset = (10, -160);
								int AlignPosition = 34;
								vec2 ClientSize = (215, 16);
								str ComponentType = "eCT_DefaultBox";
								int CurrValue = 50;
								str FrameType = "eFT_Medium";
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 71);
								int MaxValue = 200;
								int ShowProgMode = 2;
								str ThemeName = "Default";
							}

							Node "TextGuide" : "MkWindowThemedNode"
							{
								vec2 AlignOffset = (240, -20);
								int AlignPosition = 34;
								vec2 ClientSize = (110, 220);
								str ComponentType = "eCT_GuideBox";
								float LocalDepth = -1;
								vec2 LocalPosition = (243, 7);
								str ThemeName = "Default";

								Node "[PANELS]"
								{
									Node "TextTest" : "MkPanel"
									{
										uint Attribute = 26;
										float LocalDepth = -1;
										vec2 PanelSize = (110, 220);
										bool TextNodeWR = true;

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
												str Text = "[ 첫번째 제목입니당 ]";

												Node "Sub list"
												{
													int LFH = 8;
													int LFV = 3;
													str Seq = // [2]
														"일번이고" /
														"이번이구나";
													str Style = "Desc:Normal";
													str Type = "Medium";

													Node "이번이구나"
													{
														str Text = "- 평범 항목 두번째인데<LF>- 자동 개행의 유일한 방법";
													}

													Node "일번이고"
													{
														str Text = "<LF>- 평범 항목 첫번째이고<LF>";
													}
												}
											}

											Node "2st"
											{
												str Text = "<LF>[ 이거이 두번째 제목 ]";

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
														str Text = "<LF>  - 내용 항목 첫번째";
													}

													Node "1"
													{
														str Seq = // [3]
															"100" /
															"200" /
															"300";

														Node "100"
														{
															str Text = "<LF>  - 내용 항목 두번째인데 ";
														}

														Node "200"
														{
															str Style = "Desc:Highlight";
															str Text = "<% 바꿔주시용 %>";
															str Type = "Special";
														}

														Node "300"
														{
															str Text = " <- 바꾸고 강조   <LF>  - 막줄<LF>";
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			Node "Win(3)" : "MkTitleBarControlNode"
			{
				uint Attribute = 197891;
				int CaptionAlignPos = 50;
				str CaptionString = "(3) 다국어 및 깐츄롤 + 2D 라인 그리기";
				str CaptionTextName = // [3]
					"Theme" /
					"Default" /
					"Small";
				vec2 ClientSize = (344, 12);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 1;
				float LocalDepth = 200;
				vec2 LocalPosition = (100, 400);
				str ThemeName = "Default";

				Node "[SUBNODES]"
				{
					Node "BodyFrameC" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (350, 250);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.1;
						vec2 LocalPosition = (-3, -235);
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[LINES]"
						{
							Node "test red line" : "MkLineShape"
							{
								uint Attribute = 3;
								uint Color = 4294901760;
								float LDepth = -1;
							}

							Node "test white line" : "MkLineShape"
							{
								uint Attribute = 3;
								float LDepth = -1;
							}
						}

						Node "[SUBNODES]"
						{
							Node "BtnNormal" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (10, -90);
								int AlignPosition = 34;
								vec2 ClientSize = (121, 24);
								str ComponentType = "eCT_NormalBtn";
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 133);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.1;
										vec2 LocalPosition = (2.5, 3);
										str TextMsg = "개행하는 텍스트?<LF>버튼 사이즈 자동 조정";
										str TextName = // [3]
											"Theme" /
											"Default" /
											"Small";
									}
								}
							}

							Node "BtnSel B" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (10, -180);
								int AlignPosition = 34;
								vec2 ClientSize = (81, 12);
								str ComponentType = "eCT_BlueSelBtn";
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 55);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.1;
										vec2 LocalPosition = (2.5, 3);
										str TextMsg = "파랑 선택 버튼";
										str TextName = // [3]
											"Theme" /
											"Default" /
											"Small";
									}
								}
							}

							Node "BtnSel R" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (10, -210);
								int AlignPosition = 34;
								vec2 ClientSize = (117, 12);
								str ComponentType = "eCT_RedOutlineSelBtn";
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 25);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.1;
										vec2 LocalPosition = (2.5, 3);
										str TextMsg = "가이드라인 선택 버튼";
										str TextName = // [3]
											"Theme" /
											"Default" /
											"Small";
									}
								}
							}

							Node "BtnSel Y" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (10, -150);
								int AlignPosition = 34;
								vec2 ClientSize = (81, 12);
								str ComponentType = "eCT_YellowSelBtn";
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 85);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.1;
										vec2 LocalPosition = (2.5, 3);
										str TextMsg = "노랑 선택 버튼";
										str TextName = // [3]
											"Theme" /
											"Default" /
											"Small";
									}
								}
							}

							Node "CheckBox" : "MkCheckBoxControlNode"
							{
								vec2 AlignOffset = (10, -30);
								int AlignPosition = 34;
								str CaptionString = "체크박스 샘플";
								str CaptionTextName = // [3]
									"Theme" /
									"Default" /
									"Small";
								vec2 ClientSize = (12, 12);
								str ComponentType = "eCT_CheckBoxBtn";
								str FrameType = "eFT_Small";
								float LocalDepth = -1;
								vec2 LocalPosition = (13, 205);
								bool OnCheck = true;
								str ThemeName = "Default";
							}

							Node "Custom_01" : "MkWindowBaseNode"
							{
								vec2 ClientSize = (101, 101);
								str CustomFormName = "TestBtn01";
								float LocalDepth = -1;
								vec2 LocalPosition = (160, 60);
								str ThemeName = "Default";
							}

							Node "Custom_02" : "MkWindowBaseNode"
							{
								vec2 ClientSize = (101, 101);
								str CustomFormName = "TestBtn02";
								float LocalDepth = -2;
								vec2 LocalPosition = (240, 5);
								str ThemeName = "Default";
							}

							Node "DropDown" : "MkDropDownListControlNode"
							{
								vec2 AlignOffset = (10, -55);
								int AlignPosition = 34;
								vec2 ClientSize = (300, 12);
								str ComponentType = "eCT_DefaultBox";
								str FrameType = "eFT_Small";
								float LocalDepth = -2;
								vec2 LocalPosition = (13, 180);
								int MaxOnePageItemSize = 6;
								str TargetItemKey = "0";
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:ListBox" : "MkListBoxControlNode"
									{
										int AlignPosition = 82;
										uint Attribute = 67328;
										vec2 ClientSize = (300, 108);
										str ComponentType = "eCT_DefaultBox";
										str FrameType = "eFT_Small";
										str ItemKeyList = // [9]
											"0" /
											"1" /
											"2" /
											"3" /
											"4" /
											"5" /
											"6" /
											"7" /
											"8";
										str ItemMsgList = // [9]
											"일본어 : ナムウィキ、みんなで育てる知識の木。" /
											"아랍어 : ناموويكي، شجرة المعرفة التي تنمونها" /
											"번자체 : 中華人民共和國" /
											"간자체 : 中华人民共和国" /
											"신자체 : 中華人民共和国" /
											"이것도 간체자 : 这是要保存的中文字符这" /
											"태국어 :  นามุวิกิ ต้นไม้แห่งความรู้ที่พวกคุณปลูก" /
											"또 태국어 :  กรุณาเลือก Folder ที่จะดาวน์โหลดไฟล์ติดตั้ง ซึ่ง Folder [TeraInstall] จะถูกบันทึกลงในด้านล่างของ Folder ที่เลือก" /
											"키릴어 : древо знаний, которое мы выращиваем вместе";
										str ItemSequence = // [9]
											"0" /
											"1" /
											"2" /
											"3" /
											"4" /
											"5" /
											"6" /
											"7" /
											"8";
										float ItemWidth = 294;
										float LocalDepth = -0.1;
										vec2 LocalPosition = (0, -114);
										int OnePageItemSize = 6;
										str ThemeName = "Default";
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------//
// MiniKey data node text source
//   - file path : PackRoot.msd
//   - time stamp : 2019.04.24 (15:43:07)
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
				str ImagePath = // [2]
					"Image\s01.jpg" /
					"FullChange";
				float LocalDepth = 9999.000;
				vec2 PanelSize = (909.000, 505.000);
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

						Node "ShaderEffect"
						{
							str EffectName = "ttt";
							str ImagePath = // [2]
								"Image\s01.jpg" /
								"FullChange";
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
				vec2 ClientSize = (188.000, 12.000);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 3;
				vec2 LocalPosition = (500.000, 300.000);
				str ThemeName = "Default";
				bool UseCloseBtn = false;

				Node "[SUBNODES]"
				{
					Node "__#WC:BodyFrame" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (194.000, 104.000);
						str ComponentType = "eCT_NoticeBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[SUBNODES]"
						{
							Node "__#WC:Cancel" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (100.000, 8.000);
								int AlignPosition = 66;
								vec2 ClientSize = (80.000, 12.000);
								str ComponentType = "eCT_CancelBtn";
								float LocalDepth = -1.000;
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.100;
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
								vec2 AlignOffset = (32.500, 34.000);
								int AlignPosition = 66;
								float LocalDepth = -0.100;
								str TextMsg = "너무너무 잘생겨서<LF>어케 할지를 모르겠으용<LF>~(-_-)~";
								str TextName = // [3]
									"Theme" /
									"Default" /
									"Small";
							}

							Node "__#WC:Ok" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (8.000, 8.000);
								int AlignPosition = 66;
								vec2 ClientSize = (80.000, 12.000);
								str ComponentType = "eCT_OKBtn";
								float LocalDepth = -1.000;
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.100;
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
				vec2 ClientSize = (344.000, 12.000);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 1;
				float LocalDepth = 733.333;
				vec2 LocalPosition = (400.000, 600.000);
				str ThemeName = "Default";

				Node "[SUBNODES]"
				{
					Node "BodyFrame" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (350.000, 250.000);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						vec2 LocalPosition = (-3.000, -235.000);
						str ThemeName = "Default";
						bool UseShadow = true;

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
													uint Attribute = 197891;
													int CaptionAlignPos = 50;
													str CaptionString = "서브 윈도우";
													str CaptionTextName = // [3]
														"Theme" /
														"Default" /
														"Small";
													vec2 ClientSize = (244.000, 12.000);
													str ComponentType = "eCT_Title";
													str FrameType = "eFT_Small";
													int IconType = 1;
													float LocalDepth = 200.000;
													vec2 LocalPosition = (0.000, 162.000);
													str ThemeName = "Default";

													Node "[SUBNODES]"
													{
														Node "SBodyFrame" : "MkBodyFrameControlNode"
														{
															int AlignPosition = 82;
															uint Attribute = 67329;
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
																	str FrameType = "eFT_Small";
																	bool History = true;
																	float LocalDepth = -1.000;
																	vec2 LocalPosition = (13.000, 235.000);
																	str TextString = "아기 다리";
																	str ThemeName = "Default";
																}

																Node "Slider" : "MkSliderControlNode"
																{
																	vec2 AlignOffset = (10.000, -100.000);
																	int AlignPosition = 34;
																	int BeginValue = 10;
																	vec2 ClientSize = (210.000, 0.000);
																	str ComponentType = "eCT_GuideLine";
																	int CurrentValue = 20;
																	str FrameType = "eFT_Small";
																	bool Horizontal = true;
																	float LineLength = 210.000;
																	float LocalDepth = -1.000;
																	vec2 LocalPosition = (13.000, 197.000);
																	bool ShowValue = true;
																	str ThemeName = "Default";
																	int ValueSize = 90;
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
										vec2 ClientSize = (0.000, 150.000);
										str ComponentType = "eCT_GuideLine";
										int CurrentValue = 5;
										str FrameType = "eFT_Small";
										float LineLength = 150.000;
										float LocalDepth = -1.000;
										vec2 LocalPosition = (13.000, 22.000);
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
				vec2 ClientSize = (354.000, 12.000);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 1;
				float LocalDepth = 466.667;
				vec2 LocalPosition = (200.000, 500.000);
				str ThemeName = "Default";

				Node "[SUBNODES]"
				{
					Node "BodyFrameB" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (360.000, 250.000);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						vec2 LocalPosition = (-3.000, -235.000);
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[SUBNODES]"
						{
							Node "ListBox" : "MkListBoxControlNode"
							{
								vec2 AlignOffset = (10.000, -30.000);
								int AlignPosition = 34;
								vec2 ClientSize = (206.000, 108.000);
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
								float ItemWidth = 200.000;
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 109.000);
								int OnePageItemSize = 6;
								str ThemeName = "Default";
							}

							Node "ProgBar" : "MkProgressBarNode"
							{
								vec2 AlignOffset = (10.000, -160.000);
								int AlignPosition = 34;
								vec2 ClientSize = (215.000, 16.000);
								str ComponentType = "eCT_DefaultBox";
								int CurrValue = 50;
								str FrameType = "eFT_Medium";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 71.000);
								int MaxValue = 200;
								int ShowProgMode = 2;
								str ThemeName = "Default";
							}

							Node "TextGuide" : "MkWindowThemedNode"
							{
								vec2 AlignOffset = (240.000, -20.000);
								int AlignPosition = 34;
								vec2 ClientSize = (110.000, 220.000);
								str ComponentType = "eCT_GuideBox";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (243.000, 7.000);
								str ThemeName = "Default";

								Node "[PANELS]"
								{
									Node "TextTest" : "MkPanel"
									{
										uint Attribute = 26;
										float LocalDepth = -1.000;
										vec2 PanelSize = (110.000, 220.000);
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
				vec2 ClientSize = (344.000, 12.000);
				str ComponentType = "eCT_Title";
				str FrameType = "eFT_Small";
				int IconType = 1;
				float LocalDepth = 200.000;
				vec2 LocalPosition = (100.000, 400.000);
				str ThemeName = "Default";

				Node "[SUBNODES]"
				{
					Node "BodyFrameC" : "MkBodyFrameControlNode"
					{
						int AlignPosition = 34;
						uint Attribute = 66305;
						vec2 ClientSize = (350.000, 250.000);
						str ComponentType = "eCT_DefaultBox";
						int HangingType = 3;
						float LocalDepth = 0.100;
						vec2 LocalPosition = (-3.000, -235.000);
						str ThemeName = "Default";
						bool UseShadow = true;

						Node "[LINES]"
						{
							Node "test red line" : "MkLineShape"
							{
								uint Attribute = 3;
								uint Color = 4294901760;
								float LDepth = -1.000;
							}

							Node "test white line" : "MkLineShape"
							{
								uint Attribute = 3;
								float LDepth = -1.000;
							}
						}

						Node "[SUBNODES]"
						{
							Node "BtnNormal" : "MkWindowBaseNode"
							{
								vec2 AlignOffset = (10.000, -90.000);
								int AlignPosition = 34;
								vec2 ClientSize = (121.000, 24.000);
								str ComponentType = "eCT_NormalBtn";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 133.000);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.100;
										vec2 LocalPosition = (2.500, 3.000);
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
								vec2 AlignOffset = (10.000, -180.000);
								int AlignPosition = 34;
								vec2 ClientSize = (81.000, 12.000);
								str ComponentType = "eCT_BlueSelBtn";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 55.000);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.100;
										vec2 LocalPosition = (2.500, 3.000);
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
								vec2 AlignOffset = (10.000, -210.000);
								int AlignPosition = 34;
								vec2 ClientSize = (117.000, 12.000);
								str ComponentType = "eCT_RedOutlineSelBtn";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 25.000);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.100;
										vec2 LocalPosition = (2.500, 3.000);
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
								vec2 AlignOffset = (10.000, -150.000);
								int AlignPosition = 34;
								vec2 ClientSize = (81.000, 12.000);
								str ComponentType = "eCT_YellowSelBtn";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 85.000);
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:OAOTag" : "MkWindowTagNode"
									{
										int AlignPosition = 51;
										float LocalDepth = -0.100;
										vec2 LocalPosition = (2.500, 3.000);
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
								vec2 AlignOffset = (10.000, -30.000);
								int AlignPosition = 34;
								str CaptionString = "체크박스 샘플";
								str CaptionTextName = // [3]
									"Theme" /
									"Default" /
									"Small";
								vec2 ClientSize = (12.000, 12.000);
								str ComponentType = "eCT_CheckBoxBtn";
								str FrameType = "eFT_Small";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (13.000, 205.000);
								bool OnCheck = true;
								str ThemeName = "Default";
							}

							Node "Custom_01" : "MkWindowBaseNode"
							{
								vec2 ClientSize = (101.000, 101.000);
								str CustomFormName = "TestBtn01";
								float LocalDepth = -1.000;
								vec2 LocalPosition = (160.000, 60.000);
								str ThemeName = "Default";
							}

							Node "Custom_02" : "MkWindowBaseNode"
							{
								vec2 ClientSize = (101.000, 101.000);
								str CustomFormName = "TestBtn02";
								float LocalDepth = -2.000;
								vec2 LocalPosition = (240.000, 5.000);
								str ThemeName = "Default";
							}

							Node "DropDown" : "MkDropDownListControlNode"
							{
								vec2 AlignOffset = (10.000, -55.000);
								int AlignPosition = 34;
								vec2 ClientSize = (300.000, 12.000);
								str ComponentType = "eCT_DefaultBox";
								str FrameType = "eFT_Small";
								float LocalDepth = -2.000;
								vec2 LocalPosition = (13.000, 180.000);
								int MaxOnePageItemSize = 6;
								str TargetItemKey = "0";
								str ThemeName = "Default";

								Node "[SUBNODES]"
								{
									Node "__#WC:ListBox" : "MkListBoxControlNode"
									{
										int AlignPosition = 82;
										uint Attribute = 67328;
										vec2 ClientSize = (300.000, 108.000);
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
										float ItemWidth = 294.000;
										float LocalDepth = -0.100;
										vec2 LocalPosition = (0.000, -114.000);
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
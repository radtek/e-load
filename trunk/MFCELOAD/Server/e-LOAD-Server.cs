using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Xml;

namespace Server
{
	public partial class frmeLOAD : Form
	{
		private ListViewColumnSorter lvwColumnSorter;

		public frmeLOAD()
		{
			InitializeComponent();

			textURLBox.Text = "http://203.226.5.86/DownLoad/ELOAD/updateinfo.xml";
			///textURLBox.Text = "http://www.solutionware.co.kr/Projects/IDMS/users.xml";
			listUser.Columns.Add("이름");
			listUser.Columns.Add("물리 주소");
			listUser.Columns[0].Width = 150;
			listUser.Columns[1].Width = 380;

			this.Text += " ver " + Application.ProductVersion;

			// Create an instance of a ListView column sorter and assign it 
			// to the ListView control.
			lvwColumnSorter = new ListViewColumnSorter();
			this.listUser.ListViewItemSorter = lvwColumnSorter;
		}

		private void buttonGet_Click(object sender, EventArgs e)
		{
			if ("" != textURLBox.Text)
			{
				// Removes all items in the list.
				for (int i = listUser.Items.Count - 1; i >= 0; --i)
				{
					listUser.Items[i].Remove();
				}

				string rPrevName = "";
				ListViewItem listItem = new ListViewItem();
				XmlTextReader tr = new XmlTextReader(textURLBox.Text);
				while (tr.Read())
				{
					if (tr.NodeType == XmlNodeType.Element)
					{
						if (tr.AttributeCount > 0)
						{
							for (int i = 0; i < tr.AttributeCount; ++i)
							{
								tr.MoveToAttribute(i);
								if ("USER" == tr.Name)
								{
									listItem = listUser.Items.Add(tr.Value);
								}
							}
						}
					}
					else if (tr.NodeType == XmlNodeType.Text)
					{
						if ("VERSION" == rPrevName)
						{
							textVersionBox.Text = tr.Value;
						}
						else if ("INFO" == rPrevName)
						{
							///textVersionBox.Text = tr.Value;
						}
						else if (rPrevName == "USER")
						{
							listItem.SubItems.Add(tr.Value);
						}
					}
					else if (tr.NodeType == XmlNodeType.EndElement)
					{
					}
					rPrevName = tr.Name;
				}
				tr.Close();
			}
			else
			{
				MessageBox.Show("URL이 비어 있습니다.");
			}
		}

		/**
		 * @brief  
		 * 
		 * @author HumKyung.BAEK
		 * 
		 * @date   2010.08.13
		**/
		private void buttonDelete_Click(object sender, EventArgs e)
		{
			for (int i = listUser.Items.Count - 1; i >= 0; --i)
			{
				if (listUser.Items[i].Checked)
				{
					listUser.Items[i].Remove();
				}
			}
		}

		private void buttonAdd_Click(object sender, EventArgs e)
		{
			if (("" != textNameBox.Text) && ("" != textAddressBox.Text))
			{
				ListViewItem listViewItem = listUser.Items.Add(textNameBox.Text);
				listViewItem.SubItems.Add(textAddressBox.Text);
			}
			else
			{
				MessageBox.Show("이름 혹은 물리 주소 값이 비어 있습니다.");
			}
		}

		/**
		 * @brief  리스트에 있는 내용을 xml 파일로 저장한다.
		 * 
		 * @author HumKyung.BAEK
		 * 
		 * @date   2010.08.13
		**/
		private void buttonSave_Click(object sender, EventArgs e)
		{
			string aLine = "";

			Encoding unicode = Encoding.UTF8;
			try
			{
				int euckrCodePage = 51949;
				Encoding euckr = Encoding.GetEncoding(euckrCodePage);
				FileStream fs = new FileStream("updateinfo.xml", FileMode.Create);
				StreamWriter s = new StreamWriter(fs, euckr);
				s.WriteLine("<?xml version=\"1.0\" encoding=\"euc-kr\"?>");
				s.WriteLine("<UPDATEINFO>");
				s.WriteLine("<GENERAL>");
				s.WriteLine("<VERSION>" + textVersionBox.Text + "</VERSION>");
				s.WriteLine("</GENERAL>");
				s.WriteLine("<DOWNLOADS>");
				aLine = "<FILE name=\"ELECTRICAL LOAD DATABASE\" location=\"%app%\\Setup ELoad_NO_DATABASE_" +
					textVersionBox.Text + ".exe\" version=\"" + textVersionBox.Text + "\" hash=\"\" check=\"version\" section=\"\" date=\"\">";
				s.WriteLine(aLine);
				s.WriteLine("<ACTION type=\"download\">");
				s.WriteLine("<LOCATION>http://203.226.5.86/DownLoad/ELOAD/Setup ELoad_NO_DATABASE_" + textVersionBox.Text + ".exe</LOCATION>");
				s.WriteLine("</ACTION>");
				s.WriteLine("</FILE>");
				s.WriteLine("</DOWNLOADS>");
				s.WriteLine("<POST>");
				s.WriteLine("<ACTION type=\"run\">");
				s.WriteLine("<LOCATION>%updatertemp%\\Setup ELoad_NO_DATABASE_" + textVersionBox.Text + ".exe</LOCATION>");
				s.WriteLine("</ACTION>");
				s.WriteLine("</POST>");

				s.WriteLine("<SECURITY>");
				for (int i = 0; i < listUser.Items.Count; ++i)
				{
					aLine = "<CODE USER='" + listUser.Items[i].Text + "'>" + listUser.Items[i].SubItems[1].Text + "</CODE>";
					s.WriteLine(aLine);
				}
				s.WriteLine("</SECURITY>");
				s.WriteLine("</UPDATEINFO>");

				s.Close();
				fs.Close();

				MessageBox.Show("저장 완료.");
			}
			catch (IOException es)
			{
				MessageBox.Show(es.Message);
			}
		}

		private void frmeLOAD_Load(object sender, EventArgs e)
		{

		}

		/// <summary>
		/// sorting column when user click on column
		/// </summary>
		/// <author>humkyung</author>
		/// <date>2013.02.21</date>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void listUser_ColumnClick(object sender, ColumnClickEventArgs e)
		{
			// Determine if clicked column is already the column that is being sorted.
			if (e.Column == lvwColumnSorter.SortColumn)
			{
				// Reverse the current sort direction for this column.
				if (lvwColumnSorter.Order == SortOrder.Ascending)
				{
					lvwColumnSorter.Order = SortOrder.Descending;
				}
				else
				{
					lvwColumnSorter.Order = SortOrder.Ascending;
				}
			}
			else
			{
				// Set the column number that is to be sorted; default to ascending.
				lvwColumnSorter.SortColumn = e.Column;
				lvwColumnSorter.Order = SortOrder.Ascending;
			}

			// Perform the sort with these new sort options.
			this.listUser.Sort();
		}
	}
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Jotunheimr2
{
    public partial class Form1 : Form
    {
        string root;
        int rootlength;
        List<PackItem> PackItems = new List<PackItem>();

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
                textBox1.Text = folderBrowserDialog1.SelectedPath;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(textBox1.Text))
            {
                button1.Enabled = false;
                button2.Enabled = false;
                root = textBox1.Text;
                rootlength = root.Length;
                openFileDialog1.InitialDirectory = root;
                openFileDialog2.InitialDirectory = root;
                button3.Enabled = true;
                button4.Enabled = true;
                button5.Enabled = true;
                listBox1.Enabled = true;
                textBox2.Enabled = true;
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (openFileDialog1.FileName.Contains(root))
                {
                    string path = openFileDialog1.FileName.Substring(rootlength);
                    PackItem item = new PackItem();
                    item.name = openFileDialog1.SafeFileName;
                    item.path = path;
                    PackItems.Add(item);
                    listBox1.Items.Add(path);
                    listBox1.SelectedIndex = listBox1.Items.Count - 1;
                    openFileDialog1.InitialDirectory = openFileDialog1.FileName;
                    openFileDialog1.FileName = "";
                }
                else
                {
                    MessageBox.Show("File must be under the root directory");
                }
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex >= 0)
            {
                PackItem item = PackItems[listBox1.SelectedIndex];
                label3.Text = item.name;
            }
            else
            {
                label3.Text = "";
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            PackItems.Clear();
            label3.Text = "";
        }

        private void listBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                if (listBox1.SelectedIndex >= 0 && listBox1.Items.Count > 0)
                {
                    int index = listBox1.SelectedIndex;
                    PackItems.RemoveAt(index);
                    listBox1.Items.RemoveAt(index);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                if (openFileDialog2.ShowDialog() == DialogResult.OK)
                {
                    FileStream fs = File.Create(openFileDialog2.FileName);
                    fs.Write(new byte[] { 80, 65, 67, 75 }, 0, 4); //PACK
                    fs.WriteByte((byte)textBox2.Text.Length);
                    fs.Write(Encoding.UTF8.GetBytes(textBox2.Text), 0, textBox2.Text.Length);
                    int count = PackItems.Count;
                    fs.Write(BitConverter.GetBytes(count), 0, 1);
                    for (int i = 0; i < count; i++)
                    {
                        PackItem item = PackItems[i];
                        fs.WriteByte((byte)item.path.Length);
                        fs.Write(Encoding.UTF8.GetBytes(item.path), 0, item.path.Length);
                    }
                    fs.Flush();
                    fs.Close();
                    openFileDialog2.InitialDirectory = openFileDialog1.FileName;
                    openFileDialog2.FileName = "";
                    MessageBox.Show("Written successfully");
                }
            }
            catch (Exception z) { MessageBox.Show("Error"); }
        }
    }
}

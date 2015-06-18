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

namespace Jotunheimr1
{
    public partial class Form1 : Form
    {
        string root;
        int rootlength;
        List<AnimItem> AnimItems = new List<AnimItem>();

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
                checkBox1.Enabled = true;
                textBox2.Enabled = true;
                textBox3.Enabled = true;
                textBox4.Enabled = true;
                textBox5.Enabled = true;
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if(openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (openFileDialog1.FileName.Contains(root))
                {
                    string path = openFileDialog1.FileName.Substring(rootlength);
                    AnimItem item = new AnimItem();
                    item.name = openFileDialog1.SafeFileName;
                    item.path = path;
                    AnimItems.Add(item);
                    listBox1.Items.Add(path);
                    listBox1.SelectedIndex = listBox1.Items.Count - 1;
                    openFileDialog1.InitialDirectory = openFileDialog1.FileName;
                    openFileDialog1.FileName = "";
                }
                else
                    MessageBox.Show("File must be under the root directory");
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex >= 0)
            {
                AnimItem item = AnimItems[listBox1.SelectedIndex];
                label5.Text = item.name;
                textBox3.Text = item.x.ToString();
                textBox4.Text = item.y.ToString();
                textBox5.Text = item.delay.ToString();
                textBox3.Enabled = true;
                textBox4.Enabled = true;
                textBox5.Enabled = true;
                button6.Enabled = true;
            }
            else
            {
                label5.Text = "";
                textBox3.Text = "";
                textBox4.Text = "";
                textBox5.Text = "";
                textBox3.Enabled = false;
                textBox4.Enabled = false;
                textBox5.Enabled = false;
                button6.Enabled = false;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            AnimItems.Clear();
            label5.Text = "";
            textBox3.Text = "";
            textBox4.Text = "";
            textBox5.Text = "";
            textBox3.Enabled = false;
            textBox4.Enabled = false;
            textBox5.Enabled = false;
            button6.Enabled = false;
        }

        private void button6_Click(object sender, EventArgs e)
        {
            AnimItem item = AnimItems[listBox1.SelectedIndex];
            int x = -1;
            int y = -1;
            int delay = -1;
            if (int.TryParse(textBox3.Text, out x))
                if (0 <= x && x <= 65535)
                    item.x = x;
            if (int.TryParse(textBox4.Text, out y))
                if (0 <= y && y <= 65535)
                    item.y = y;
            if (int.TryParse(textBox5.Text, out delay))
                if (0 <= delay && delay <= 65535)
                    item.delay = delay;
            if (item.x != x || item.y != y || item.delay != delay)
                MessageBox.Show("Error");
            else
                MessageBox.Show("Saved");
        }

        private void textBox3_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                button6_Click(null, null);
        }

        private void textBox4_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                button6_Click(null, null);
        }

        private void textBox5_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                button6_Click(null, null);
        }

        private void listBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                if (listBox1.SelectedIndex >= 0 && listBox1.Items.Count > 0)
                {
                    int index = listBox1.SelectedIndex;
                    AnimItems.RemoveAt(index);
                    listBox1.Items.RemoveAt(index);
                }
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            try
            {
                if (openFileDialog2.ShowDialog() == DialogResult.OK)
                {
                    FileStream fs = File.Create(openFileDialog2.FileName);
                    fs.Write(new byte[] { 65, 78, 73, 77 }, 0, 4); //ANIM
                    fs.WriteByte((byte)textBox2.Text.Length);
                    fs.Write(Encoding.UTF8.GetBytes(textBox2.Text), 0, textBox2.Text.Length);
                    fs.WriteByte((byte)AnimItems.Count);
                    fs.WriteByte((byte)(checkBox1.Checked ? 255 : 0));
                    int count = AnimItems.Count;
                    for (int i = 0; i < count; i++)
                    {
                        AnimItem item = AnimItems[i];
                        fs.WriteByte((byte)item.path.Length);
                        fs.Write(Encoding.UTF8.GetBytes(item.path), 0, item.path.Length);
                        fs.Write(BitConverter.GetBytes(item.x), 0, 2);
                        fs.Write(BitConverter.GetBytes(item.y), 0, 2);
                        fs.Write(BitConverter.GetBytes(item.delay), 0, 2);
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

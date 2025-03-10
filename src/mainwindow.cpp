//
// Created by felipe on 11/12/2021.
//
#include "mainwindow.h"

MainWindow::~MainWindow ()
= default;

ListModelColumns::ListModelColumns ()
{
  add(m_col_id);
  add(m_col_name);
  add(m_col_value);
  //add(m_col_percentage);
}

MainWindow::MainWindow ()
    : m_VBox(Gtk::Orientation::VERTICAL)
{
  set_layout();
  set_slots();

  add_column("Id", m_columns.m_col_id);
  add_column("Arquivo", m_columns.m_col_name);

  add_column ("Tamanho", m_columns.m_col_value);

  m_treeView.get_column (m_columns.m_col_name.index())->set_expand (true);
  m_treeView.get_column (m_columns.m_col_name.index())->set_sizing (Gtk::TreeViewColumn::Sizing::AUTOSIZE);
  //m_treeView.get_column (m_columns.m_col_name.index())->set_min_width(600);

  m_treeView.get_column (2)->set_expand (false);
  m_treeView.get_column (2)->set_sizing (Gtk::TreeViewColumn::Sizing::FIXED);
  m_treeView.get_column (2)->set_min_width(160);
  //insere a barra de progresso para o campo percent
  /*auto cell = Gtk::make_managed<Gtk::CellRendererProgress>();
  int cols_count = m_treeView.append_column("Restante", *cell);
  auto pColumn = m_treeView.get_column(cols_count - 1);
  if(pColumn)
    {
      pColumn->add_attribute(cell->property_value(), m_columns.m_col_percentage);
    }
*/
}
void MainWindow::set_layout ()
{
  set_title("Cryptographer");
  set_default_size(1000, 600);

  m_VBox.set_margin(5);
  set_child(m_VBox);

  //Adiciona a treeview dentro do scroll
  m_refTreeModel = Gtk::ListStore::create(m_columns);
  m_treeView.set_model(m_refTreeModel);
  m_treeView.set_headers_clickable (false);


  m_button_Add.set_label ("Adicionar");
  m_button_Compress.set_label ("Criptografar");
  m_button_Decompress.set_label ("Descriptografar");
  m_button_Quit.set_label ("Fechar");
  m_buttonBox.set_spacing (33);
  m_entry_Chave.set_max_length (16);
  m_entry_Chave.set_tooltip_text ("Chave de 16 Bytes para Criptografia");
  m_entry_Chave.set_placeholder_text ("Insira a Chave");
  m_entry_Chave.set_margin_start (5);
  m_buttonBox.append (m_entry_Chave);
  m_buttonBox.append (m_button_Add);
  m_buttonBox.append (m_button_Compress);
  m_buttonBox.append (m_button_Decompress);
  m_buttonBox.append(m_button_Quit);
  m_buttonBox.set_margin(5);
  m_button_Quit.set_hexpand(true);

  m_button_Quit.set_halign(Gtk::Align::END);
  m_button_Compress.set_halign (Gtk::Align::END);
  m_button_Add.set_halign (Gtk::Align::END);

  m_VBox.append (m_buttonBox);
  m_scrolledWindow.set_child(m_treeView);
  //Mostra scrollbar somente quando é necessario
  m_scrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  m_scrolledWindow.set_expand();
  m_VBox.append(m_scrolledWindow);
}

void MainWindow::set_slots ()
{
  m_button_Quit.signal_clicked().connect( sigc::mem_fun(*this,&MainWindow::on_click_m_Button_Quit));
  m_button_Add.signal_clicked().connect (sigc::mem_fun (*this, &MainWindow::on_click_m_Button_Add));
  m_button_Compress.signal_clicked().connect (sigc::mem_fun (*this, &MainWindow::on_click_m_Button_Compress));
  m_button_Decompress.signal_clicked().connect (sigc::mem_fun (*this, &MainWindow::on_click_m_Button_Decompress));
}

void MainWindow::add_item (unsigned int id, const Glib::ustring& name, double value, int percent)
{
  Gtk::TreeRow m_Row;
  m_Row = *(m_refTreeModel->append());
  m_Row[m_columns.m_col_id] = id;
  m_Row[m_columns.m_col_name] = name;
  m_Row[m_columns.m_col_value] = value;
  //m_Row[m_columns.m_col_percentage] = percent;

}

template<typename Type>
void MainWindow::add_column (const Glib::ustring &title, const Gtk::TreeModelColumn<Type> &m_column)
{
  m_treeView.append_column (title, m_column);
}
template<typename Type>
void MainWindow::add_column_w_mask (const Glib::ustring &title, const Gtk::TreeModelColumn<Type> &m_column, const Glib::ustring &mask)
{
  m_treeView.append_column_numeric (title, m_column, mask);
}
void MainWindow::on_click_m_Button_Add ()
{
  auto dialog = new Gtk::FileChooserDialog("Selecione um arquivo",
                                           Gtk::FileChooser::Action::OPEN);
  dialog->set_transient_for(*this);
  dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::on_file_dialog_response), dialog));

  //Adicionar botoes
  dialog->add_button("_Abrir", Gtk::ResponseType::OK);
  dialog->add_button("_Cancelar", Gtk::ResponseType::CANCEL);

  //Adiciona os filtros de arquivos
  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name(" All ");
  filter_any->add_pattern("*");
  dialog->add_filter(filter_any);

  //Mostra o dialogo e aguarda a response
  dialog->show();
}

void MainWindow::on_click_m_Button_Quit ()
{
  this->hide();
}
void MainWindow::on_file_dialog_response (int response_id, Gtk::FileChooserDialog *dialog)
{
  switch (response_id)
    {
      case Gtk::ResponseType::OK:
        {
          std::cout << "Clicou em Abrir." << std::endl;

          //retorna uma std:string
          auto filename = dialog->get_file()->get_path();
          auto arquivo = dialog->get_file()->query_info ();
          std::cout << "Arquivo Selecionado: " <<  filename << std::endl;
          add_file_list(filename, arquivo->get_size());
          break;
        }
      case Gtk::ResponseType::CANCEL:
        {
          std::cout << "Clicou em cancelar." << std::endl;
          break;
        }
      default:
        {
          std::cout << "Outro Botao." << std::endl;
          break;
        }
    }
  delete dialog;
}
void MainWindow::on_click_m_Button_Compress ()
{
  //para cada filho(item)
  if(!m_refTreeModel->children().empty())
    {
      std::array<unsigned char, 16> b_user_key = convert_to_barray (m_entry_Chave.get_text());
      std::array<unsigned char, 16> b_iv = {'0','x'};
      for(auto row : m_refTreeModel->children())
        {
          encrypt(b_user_key, b_iv, row[m_columns.m_col_name], row[m_columns.m_col_name]+"-cif");
        }
      show_finished_msg_box();
    }
}

void MainWindow::on_click_m_Button_Decompress ()
{
  if(!m_refTreeModel->children().empty())
    {
      std::array<unsigned char, 16> b_user_key = convert_to_barray (m_entry_Chave.get_text());
      std::array<unsigned char, 16> b_iv = {'0','x'};
      for(auto row : m_refTreeModel->children())
        {

          decrypt (b_user_key, b_iv, row[m_columns.m_col_name], row[m_columns.m_col_name]+".out");
        }

      show_finished_msg_box();
    }
}

void MainWindow::add_file_list (std::string &filename, unsigned long long int size)
{
  file_count++;
  //somente caso nao for windows irá introduzir um backslash e espaço (definidos por sistemas baseados em unix)
#ifndef __WIN32
  search_and_replace (filename, std::string(" "), std::string("\\ "));
#endif
  add_item (file_count,filename, size, 0);
}

void MainWindow::search_and_replace(std::basic_string<char> str, std::string const& search, std::string const& replace)
{
  std::string::size_type next;

  for(next = str.find(search);next != std::string::npos;next = str.find(search,next))
    {
      str.replace(next,search.length(),replace);
      next += replace.length();
    }
}

std::array<unsigned char, 16> MainWindow::convert_to_barray(const Glib::ustring &entry)
{
  std::array<unsigned char, 16> result = {};

  if(entry.size() <= result.size())
    {
      for(int i = 0; i < entry.size(); i++)
        {
          result[i] = entry[i];
        }
    }

  return result;
}

void MainWindow::encrypt(const aes_key_t &key, const aes_iv_t &iv,const Glib::ustring &filename_in, const Glib::ustring &filename_out)
{
  std::cout << "MainWindow::encrypt || " << key.data() << " " << iv.data() << " \nfe:" << filename_in << "\nfo:" << filename_out;
  CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cipher{};
  cipher.SetKeyWithIV(key.data(), key.size(), iv.data());

  std::ifstream in{filename_in, std::ios::binary};
  std::ofstream out{filename_out, std::ios::binary};

  CryptoPP::FileSource{in, /*bombeia todos=*/true,new CryptoPP::StreamTransformationFilter{cipher, new CryptoPP::FileSink{out}}};
}

void MainWindow::decrypt(const aes_key_t &key, const aes_iv_t &iv, const Glib::ustring &filename_in, const Glib::ustring &filename_out)
{
  std::cout << "MainWindow::decrypt || " << key.data() << " " << iv.data() << " \nfe:" << filename_in << "\nfo:" << filename_out;
  CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cipher{};
  cipher.SetKeyWithIV(key.data(), key.size(), iv.data());

  std::ifstream in{filename_in, std::ios::binary};
  std::ofstream out{filename_out, std::ios::binary};

  CryptoPP::FileSource{in, /*bombeia todos=*/true,new CryptoPP::StreamTransformationFilter{cipher, new CryptoPP::FileSink{out}}};
}
void MainWindow::show_finished_msg_box ()
{
  m_pDialog.reset(new Gtk::MessageDialog(*this, "Concluido"));
  m_pDialog->set_secondary_text("O(s) arquivos foram criptografados utilizando o algoritmo AES com o modo CFB.");
  m_pDialog->set_modal(true);
  m_pDialog->set_hide_on_close(true);
  m_pDialog->signal_response().connect(sigc::hide(sigc::mem_fun(*m_pDialog, &Gtk::Widget::hide)));

  m_pDialog->show();
}

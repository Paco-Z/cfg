/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007-2012 by TAKAGI Nobuhisa
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 */
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "toppers/misc.hpp"
#include "toppers/global.hpp"
#include "toppers/csv.hpp"
#include "toppers/nm_symbol.hpp"
#include "toppers/s_record.hpp"
#include "toppers/diagnostics.hpp"
#include "toppers/macro_processor.hpp"
#include "toppers/io.hpp"
#include "toppers/cpp.hpp"
#include "toppers/itronx/factory.hpp"
#include "toppers/itronx/cfg1_out.hpp"

namespace toppers
{
  namespace itronx
  {
    namespace
    {
      /*!
       *  \brief  オブジェクトID番号の割付け
       *  \param[in]  api_map   ソースに記述された静的APIを登録したコンテナ
       */
      void assign_id( toppers::itronx::cfg1_out::static_api_map& api_map )
      {
        using namespace toppers;
        using namespace toppers::itronx;

        std::string id_input_file( get_global_string( "id-input-file" ) );
        if ( id_input_file.empty() )  // --id-input-file オプションが指定されていない場合...
        {
          for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
                iter != last;
                ++iter )
          {
            static_api::assign_id( iter->second.begin(), iter->second.end() );
          }
        }
        else  // --id-input-file オプションが指定されている場合...
        {
          typedef std::map< std::string, std::pair< long, bool > > id_map_t;
          id_map_t id_map;
          std::ifstream ifs( id_input_file.c_str() );
          while ( ifs )
          {
            std::string linebuf;
            std::getline( ifs, linebuf );
            if ( ifs.bad() )
            {
              fatal( _( "I/O error" ) );
            }
            if ( linebuf.empty() || linebuf == "\r" )
            {
              break;
            }

            std::istringstream iss( linebuf );
            std::string name;
            iss >> name;
            if ( iss.fail() )
            {
              fatal( _( "id file `%1%\' is invalid" ), id_input_file );
            }

            long value;
            iss >> value;
            if ( iss.fail() )
            {
              fatal( _( "id file `%1%\' is invalid" ), id_input_file );
            }

            if ( id_map.find( name ) != id_map.end() )
            {
              fatal( _( "E_OBJ: `%1%\' is duplicated" ), name );
            }
            else
            {
              id_map[ name ] = std::make_pair( value, false );
            }
          }

          for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
                iter != last;
                ++iter )
          {
            for ( std::vector< static_api >::iterator iter2( iter->second.begin() ), last2( iter->second.end() );
                  iter2 != last2;
                  ++iter2 )
            {
              static_api::info const* info = iter2->get_info();
              if ( info->id_pos >= 0 )
              {
                std::string name( iter2->at( info->id_pos ).text );
                std::string symbol( iter2->at( info->id_pos ).symbol );
                if ( !info->slave && symbol[0] == '#' )
                {
                  id_map_t::iterator hit( id_map.find( name ) );
                  if ( hit != id_map.end() )
                  {
                    long id_value = hit->second.first;
                    if ( id_value > 0 )
                    {
                      iter2->at( info->id_pos ).value = id_value;
                      hit->second.second = true;
                    }
                  }
                }
              }
            }
            static_api::assign_id( iter->second.begin(), iter->second.end() );
          }

          for ( id_map_t::const_iterator iter( id_map.begin() ), last( id_map.end() ); iter != last; ++iter )  // 残り物があれば...
          {
            if ( !iter->second.second )
            {
              warning( _( "object identifier `%1%\' is not used" ), iter->first );
            }
          }
        }

        // --id-output-file オプションが指定されている場合
        std::string id_output_file( get_global_string ( "id-output-file" ) );
        if ( !id_output_file.empty() )
        {
          std::ofstream ofs( id_output_file.c_str() );
          for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
            iter != last;
            ++iter )
          {
            for ( std::vector< static_api >::const_iterator iter2( iter->second.begin() ), last2( iter->second.end() );
                  iter2 != last2;
                  ++iter2 )
            {
              static_api::info const* info = iter2->get_info();
              if ( info->id_pos >= 0 )
              {
                std::string name( iter2->at( info->id_pos ).text );
                std::string symbol( iter2->at( info->id_pos ).symbol );
                if ( !info->slave && symbol[0] == '#' )
                {
                  ofs << name << '\t' << iter2->at( info->id_pos ).value.get() << std::endl;
                }
              }
            }
          }
        }
      }

      // カーネルオブジェクト生成・定義用静的APIの各パラメータをマクロプロセッサの変数として設定する。
      void set_object_vars( cfg1_out::static_api_map const& api_map, macro_processor& mproc )
      {
        typedef macro_processor::element element;
        typedef macro_processor::var_t var_t;
        std::map< std::string, var_t > order_list_map;
        std::map< std::string, long > id_map;

        for ( cfg1_out::static_api_map::const_iterator m_iter( api_map.begin() ), m_last( api_map.end() );
              m_iter != m_last;
              ++m_iter )
        {
          int order = 1;

          for ( std::vector< static_api >::const_iterator v_iter( m_iter->second.begin() ), v_last( m_iter->second.end() );
                v_iter != v_last;
                ++v_iter )
          {
            static_api::info const* info = v_iter->get_info();

            long id = -1;
            if ( !info->slave )
            {
              // 出現順 $OBJ.ORDER$
              if ( v_iter->id().value )
              {
                id = static_cast< long >( v_iter->id().value.get() );
              }
              else
              {
                id = order;
              }
              id_map[ v_iter->id().text ] = id;
              element e;
              e.i = order;
              mproc.set_var( toppers::toupper( ( boost::format( "%s.order" ) % info->type ).str() ), id, var_t( 1, e ) );

              e.i = id;
              e.s = v_iter->id().text;
              order_list_map[ info->type ].push_back( e );
              ++order;
            }
            else
            {
              std::map< std::string, long >::iterator iter( id_map.find( v_iter->id().text ) );
              if ( iter == id_map.end() )
              {
                error( v_iter->line(), _( "E_NOEXS: `%1%\' is undefined" ), v_iter->id().text );
              }
              else
              {
                id = iter->second;
              }
            }

            // 各パラメータ
            var_t var;
            std::string prev_name;
            for ( static_api::const_iterator api_iter( v_iter->begin() ), api_last( v_iter->end() );
                  api_iter != api_last;
                  ++api_iter )
            {
              bool is_param_list = false;

              char const* type_name = ( api_iter->symbol[0] == '*'  && info->slave ) ? info->api_name : info->type;
              std::string name( toppers::toupper( ( boost::format( "%s.%s" ) % type_name % ( api_iter->symbol.c_str() + 1 ) ).str() ) );
              // 末尾の ? を除去
              if ( *name.rbegin() == '\?' ) 
              {
                name.resize( name.size() - 1 );
              }
              // 末尾の ... を除去
              if ( name.size() > 3 && name.substr( name.size() - 3 ) == "..." )
              {
                name.resize( name.size() - 3 );
                is_param_list = true;
              }

              if (!var.empty())
              {
                if (prev_name != name)
                {
                  var.clear();
                }
              }

              element e;
              e.s = api_iter->text; // ソースの字面
              if ( api_iter->symbol[0] != '&' )   // 一般定数式パラメータは値が特定できない
              {
                if ( api_iter->symbol[0] == '$' )  // 文字列定数式パラメータ
                {
                  e.v = api_iter->string; // 展開後の文字列
                }
                else
                {
                  e.i = api_iter->value;
                }
                if ( api_iter->symbol[0] == '%' )
                {
                  continue;
                }
              }
              var.push_back(e);
              mproc.set_var( name, id, var );

              prev_name = name;

              // 下位互換性のため、symbol + order 名の変数を登録
              if (is_param_list)
              {
                mproc.set_var( name + boost::lexical_cast< std::string >( api_iter->order ), id, var_t( 1, e ) );
              }
            }

            // 静的APIの名称
            if ( !info->slave ) {
              element e;
              e.s = info->api_name;
              std::string type( toppers::toupper( info->type ) );
              mproc.set_var( type + ".APINAME", id, var_t( 1, e ) );
            }

            // 静的APIが出現した行番号
            {
              element e;
              e.s = v_iter->line().file;
              e.i = v_iter->line().line;
              std::string type( toppers::toupper( info->slave ? info->api_name : info->type ) );
              mproc.set_var( type + ".TEXT_LINE", id, var_t( 1, e ) );
            }
          }
        }

        std::map< std::string, var_t > id_list_map;

        for ( std::map< std::string, var_t >::const_iterator iter( order_list_map.begin() ), last( order_list_map.end() );
              iter != last;
              ++iter )
        {
          // 出現順リスト $OBJ.ORDER_LIST$ -- ID番号の並び
          mproc.set_var( toppers::toupper( iter->first + ".order_list" ), iter->second );
          var_t rorder_list( iter->second );

          // 逆順リスト $OBJ.RORDER_LIST$ -- ID番号の並び
          std::reverse( rorder_list.begin(), rorder_list.end() );
          mproc.set_var( toppers::toupper( iter->first + ".rorder_list" ), rorder_list );

          // ID番号リスト $OBJ.ID_LIST$ -- ID番号の並び
          var_t id_list( iter->second );
          std::sort( id_list.begin(), id_list.end() );
          mproc.set_var( toppers::toupper( iter->first + ".id_list" ), id_list );
        }

        element external_id;
        external_id.i = get_global< bool >( "external-id" );
        mproc.set_var( "USE_EXTERNAL_ID", var_t( 1, external_id ) );
      }

      // カーネルオブジェクト生成・定義用静的APIの各パラメータをマクロプロセッサの変数として設定する。
      void set_object_vars( std::vector< static_api > const& api_array, macro_processor& mproc )
      {
        typedef macro_processor::element element;
        typedef macro_processor::var_t var_t;
        long order = 1;
        var_t order_list;

        for ( std::vector< static_api >::const_iterator v_iter( api_array.begin() ), v_last( api_array.end() );
              v_iter != v_last;
              ++v_iter )
        {
          static_api::info const* info = v_iter->get_info();
          var_t params;
          var_t args;

          // 静的APIが出現した行番号
          element e;
          e.s = v_iter->line().file;
          e.i = v_iter->line().line;
          mproc.set_var( "API.TEXT_LINE", order, var_t( 1, e ) );

          // 静的API名
          e.s = info->api_name;
          e.i = boost::none;
          mproc.set_var( "API.NAME", order, var_t( 1, e ) );

          // オブジェクトタイプ（"TSK", "SEM", ...）
          e.s = toppers::toupper( info->type );
          mproc.set_var( "API.TYPE", order, var_t( 1 , e ) );

          // 各パラメータ
          for ( static_api::const_iterator api_iter( v_iter->begin() ), api_last( v_iter->end() );
                api_iter != api_last;
                ++api_iter )
          {
            std::string name( toppers::toupper( ( boost::format( "%s.%s" ) % info->type % ( api_iter->symbol.c_str() + 1 ) ).str() ) );
            // 末尾の ? を除去
            if ( *name.rbegin() == '\?' ) 
            {
              name.resize( name.size() - 1 );
            }
            // 末尾の ... を除去 & order を付加
            if ( name.size() > 3 && name.substr( name.size() - 3 ) == "..." )
            {
              name.resize( name.size() - 3 );
              name += boost::lexical_cast< std::string >( api_iter->order );
            }

            element e;
            e.s = api_iter->text; // ソースの字面
            if ( api_iter->symbol[0] != '&' )   // 一般定数式パラメータは値が特定できない
            {
              if ( api_iter->symbol[0] == '$' )  // 文字列定数式パラメータ
              {
                e.v = api_iter->string; // 展開後の文字列
              }
              else
              {
                e.i = api_iter->value;
              }
            }
            args.push_back( e );

            e.s = name;
            e.i = boost::none;
            params.push_back( e );

            if ( api_iter->symbol[0] == '%' )
            {
              continue;
            }
          }
          mproc.set_var( "API.ARGS", order, args );
          mproc.set_var( "API.PARAMS", order, params );
          e.s.clear();
          e.i = order;
          order_list.push_back( e );
          ++order;
        }
        mproc.set_var( "API.ORDER_LIST", order_list );

        element external_id;
        external_id.i = get_global< bool >( "external-id" );
        mproc.set_var( "USE_EXTERNAL_ID", var_t( 1, external_id ) );
      }

      // クラスIDリストをマクロプロセッサの変数として設定する。
      void set_clsid_vars( std::vector< std::pair< std::string, long > > const& table, cfg1_out const& cfg1out, macro_processor& mproc )
      {
        if ( cfg1out.get_def_table() == 0 )
        {
          return;
        }

        typedef macro_processor::element element;
        macro_processor::var_t var;

        bool little_endian = cfg1out.is_little_endian();
        nm_symbol::entry nm_entry = cfg1out.get_syms()->find( "TOPPERS_cfg_sizeof_signed_t" );
        std::size_t sizeof_signed_t = static_cast< std::size_t >( cfg1out.get_srec()->get_value( nm_entry.address, 4, little_endian ) );

        for ( std::vector< std::pair< std::string, long > >::const_iterator iter( table.begin() ), last( table.end() );
              iter != last;
              ++iter )
        {
          if ( !iter->first.empty() )
          {
            element e;
            e.s = iter->first;

            std::string symbol = "TOPPERS_cfg_valueof_" + iter->first;
            nm_symbol::entry nm_entry = cfg1out.get_syms()->find( symbol );
            if ( nm_entry.type == -1 )
            {
              continue;
            }
            const std::size_t size = sizeof_signed_t;
            std::tr1::intmax_t value = cfg1out.get_srec()->get_value( nm_entry.address, size, little_endian );

            e.i = value;

            var.push_back( e );
          }
        }
        // クラスIDの値でソートしていない
        mproc.set_var( "CLS.ID_LIST", var );
      }

      // ドメインIDリストをマクロプロセッサの変数として設定する。
      void set_domid_vars( std::vector< std::pair< std::string, long > > const& table, macro_processor& mproc )
      {
        typedef macro_processor::element element;
        macro_processor::var_t var;

        for ( std::vector< std::pair< std::string, long > >::const_iterator iter( table.begin() ), last( table.end() );
              iter != last;
              ++iter )
        {
          if ( !iter->first.empty() )
          {
            element e;
            e.s = iter->first;
            e.i = iter->second;
            var.push_back( e );
          }
        }
        mproc.set_var( "DOM.ID_LIST", var );
      }

      // プラットフォーム・コンパイラ依存の値をマクロプロセッサの変数として設定する。
      void set_platform_vars( cfg1_out const& cfg1out, macro_processor& mproc )
      {
        typedef macro_processor::element element;
        typedef macro_processor::var_t var_t;

        cfg1_out::cfg1_def_table const* def_table = cfg1out.get_def_table();
        if ( def_table == 0 )
        {
          return;
        }
        std::size_t sizeof_signed_t;
        std::size_t sizeof_pointer;

        static cfg1_out::cfg1_def_t const limit_defs[] =
        {
          { false, "TOPPERS_cfg_CHAR_BIT",  "CHAR_BIT" },
          { false, "TOPPERS_cfg_CHAR_MAX",  "CHAR_MAX" },
          { true, "TOPPERS_cfg_CHAR_MIN",  "CHAR_MIN" },
          { false, "TOPPERS_cfg_SCHAR_MAX", "SCHAR_MAX" },  // 本来は符号付きだが、負になることはない
          { false, "TOPPERS_cfg_SHRT_MAX",  "SHRT_MAX" },   // 本来は符号付きだが、負になることはない
          { false, "TOPPERS_cfg_INT_MAX",   "INT_MAX" },    // 本来は符号付きだが、負になることはない
          { false, "TOPPERS_cfg_LONG_MAX",  "LONG_MAX" },   // 本来は符号付きだが、負になることはない
        };

        nm_symbol::entry nm_entry = cfg1out.get_syms()->find( "TOPPERS_cfg_sizeof_signed_t" );
        sizeof_signed_t = static_cast< std::size_t >( cfg1out.get_srec()->get_value( nm_entry.address, 4, cfg1out.is_little_endian() ) );

        nm_entry = cfg1out.get_syms()->find( "TOPPERS_cfg_sizeof_pointer" );
        sizeof_pointer = static_cast< std::size_t >( cfg1out.get_srec()->get_value( nm_entry.address, 4, cfg1out.is_little_endian() ) );

        for ( int i = 0; i < sizeof(limit_defs) / sizeof(limit_defs[ 0 ]); ++i )
        {
          element e;
          e.s = limit_defs[ i ].expression;
          nm_entry = cfg1out.get_syms()->find( limit_defs[ i ].name );
          std::tr1::int64_t value = cfg1out.get_srec()->get_value( nm_entry.address, sizeof_signed_t, cfg1out.is_little_endian() );
          if ( sizeof_signed_t < 8 && limit_defs[ i ].is_signed )
          {
            value = cfg1_out::make_signed( static_cast< std::tr1::uint32_t >( value ) );
          }
          mproc.set_var( e.s, var_t( 1, e ) );
        }

        for ( cfg1_out::cfg1_def_table::const_iterator iter( def_table->begin() ), last( def_table->end() );
              iter != last;
              ++iter )
        {
          element e;
          std::tr1::int64_t value;

          nm_entry = cfg1out.get_syms()->find( "TOPPERS_cfg_" + iter->name );
          if ( nm_entry.type >= 0 )
          {
            if ( !iter->expression.empty() && iter->expression[ 0 ] == '@' )  // 式が'@'で始まる場合はアドレス定数式
            {
              value = cfg1out.get_srec()->get_value( nm_entry.address, sizeof_pointer, cfg1out.is_little_endian() );
              if ( sizeof_signed_t < 8 && iter->is_signed )
              {
                value = cfg1_out::make_signed( static_cast< std::tr1::uint32_t >( value ) );
              }

              // 先ほど取り出したアドレスを使って間接参照
              value = cfg1out.get_srec()->get_value( value, 8, cfg1out.is_little_endian() );  // 取り出す値は型に関係なく常に8バイト
              if ( sizeof_signed_t < 8 && iter->is_signed )
              {
                value = cfg1_out::make_signed( static_cast< std::tr1::uint32_t >( value ) );
              }
              e.s = iter->expression.c_str() + 1; // 先頭の'@'を除去
            }
            else  // アドレスではない通常の整数定数式
            {
              value = cfg1out.get_srec()->get_value( nm_entry.address, sizeof_signed_t, cfg1out.is_little_endian() );
              if ( sizeof_signed_t < 8 && iter->is_signed )
              {
                value = cfg1_out::make_signed( static_cast< std::tr1::uint32_t >( value ) );
              }
              e.s = iter->expression;
            }
            e.i = value;
            mproc.set_var( iter->name, var_t( 1, e ) );
          }
        }

        // バイトオーダー
        {
          bool little_endian = cfg1out.is_little_endian();
          element e;
          e.i = little_endian;
          mproc.set_var( "LITTLE_ENDIAN", var_t( 1, e ) );

          e.i = !little_endian;
          mproc.set_var( "BIG_ENDIAN", var_t( 1, e ) );
        }
      }

    }

    //! コンストラクタ
    factory::factory( std::string const& kernel )
      : kernel_( tolower( kernel ) )
    {
    }

    //! デストラクタ
    factory::~factory()
    {
    }

    std::unique_ptr< macro_processor > factory::create_macro_processor( cfg1_out const& cfg1out ) const
    {
      cfg1_out::static_api_map api_map( cfg1out.merge() );
      assign_id( api_map );
      return do_create_macro_processor( cfg1out, api_map );
    }

    //! サポートしている静的API情報の取得
    std::map< std::string, static_api::info > const* factory::get_static_api_info_map() const
    {
      // CSVから静的API情報を読み取り、登録するためのローカルクラス
      struct init_t
      {
        init_t()
        {
          boost::any t = global( "api-table" );
          if ( !t.empty() )
          {
            std::vector< std::string > api_tables( boost::any_cast< std::vector< std::string >& >( t ) );
            for ( std::vector< std::string >::const_iterator iter( api_tables.begin() ), last( api_tables.end() );
                  iter != last;
                  ++iter )
            {
              std::string buf;
              std::string api_table_filename = *iter;
              read( api_table_filename.c_str(), buf );
              csv data( buf.begin(), buf.end() );
              for ( csv::const_iterator d_iter( data.begin() ), d_last( data.end() );
                    d_iter != d_last;
                    ++d_iter )
              {
                csv::size_type len = d_iter->size();
                if ( len < 3 )  // type, api_name, params は必須要素
                {
                  toppers::fatal( _( "too little fields in `%1%\'" ), *iter );
                }
                static_api::info api_info = { 0 };
                try
                {
                    char* s;
                    s = new char[ ( *d_iter )[ 0 ].size() + 1 ];
                    std::strcpy( s, ( *d_iter )[ 0 ].c_str() );
                    api_info.type = s;

                    s = new char[ ( *d_iter )[ 1 ].size() + 1 ];
                    std::strcpy( s, ( *d_iter )[ 1 ].c_str() );
                    api_info.api_name = s;

                    s = new char[ ( *d_iter )[ 2 ].size() + 1 ];
                    std::strcpy( s, ( *d_iter )[ 2 ].c_str() );
                    api_info.params = s;
                    if ( len >= 4 && !( *d_iter )[ 3 ].empty() )
                    {
                      api_info.id_pos = std::strtol( ( *d_iter )[ 3 ].c_str(), 0, 0 );
                    }
                    if ( len >= 5 && !( *d_iter )[ 4 ].empty() )
                    {
                      api_info.slave = !!std::strtol( ( *d_iter )[ 4 ].c_str(), 0, 0 );
                    }
                    static_api_table_[ api_info.api_name ] = api_info;
                }
                catch ( ... )
                {
                  delete[] api_info.type;
                  delete[] api_info.api_name;
                  delete[] api_info.params;
                  throw;
                }
              }
            }
          }
        }

        ~init_t()
        {
          for ( std::map< std::string, static_api::info >::const_iterator iter( static_api_table_.begin() ), last( static_api_table_.end() );
                iter != last;
                ++iter )
          {
            delete[] iter->second.type;
            delete[] iter->second.api_name;
            delete[] iter->second.params;
          }
        }

        std::map< std::string, static_api::info > static_api_table_;
      };
      static init_t init;
      std::map< std::string, static_api::info > const* result = &init.static_api_table_;
      return result;
    }

    /*!
     * \brief   cfg1_out.c への出力情報テーブルの生成
     * \return  生成した cfg1_out::cfg1_def_table オブジェクトへのポインタ
     * \note    この関数が返すポインタは delete してはならない
     *
     * --cfg1-def-table オプションで指定したファイルから、cfg1_out.c へ出力する情報を読み取り、
     * cfg1_out::cfg1_def_table オブジェクトを生成する。
     *
     * CSV の形式は以下の通り
     *
     *    シンボル名,式[,s|signed]
     *
     * 末尾の s または signed は省略可能。省略時は符号無し整数とみなす。s または signed 指定時は
     * 符号付き整数とみなす。\n
     * 「式」の最初に # があれば前処理式とみなす。
     */
    cfg1_out::cfg1_def_table const* factory::get_cfg1_def_table() const
    {
      struct init_t
      {
        init_t()
        {
          boost::any t = global( "cfg1-def-table" );
          if ( t.empty() )
          {
            without_def_table_ = true;
          }
          else
          {
            without_def_table_ = false;

            std::vector< std::string > cfg1_def_table = boost::any_cast< std::vector< std::string >& >( t );
            for ( std::vector< std::string >::const_iterator iter( cfg1_def_table.begin() ), last( cfg1_def_table.end() );
                  iter != last;
                  ++iter )
            {
              std::string buf;
              read( iter->c_str(), buf );
              csv data( buf.begin(), buf.end() );
              for ( csv::const_iterator d_iter( data.begin() ), d_last( data.end() );
                    d_iter != d_last;
                    ++d_iter )
              {
                csv::size_type len = d_iter->size();
                if ( len < 2 )
                {
                  toppers::fatal( _( "too little fields in `%1%\'" ), *iter );
                }
                cfg1_out::cfg1_def_t def = { 0 };
                def.name = ( *d_iter )[ 0 ];
                def.expression = ( *d_iter )[ 1 ];
                if ( len >= 3 )
                {
                  std::string is_signed( ( *d_iter )[ 2 ] );
                  def.is_signed = ( is_signed == "s" || is_signed == "signed" );
                }
                if ( len >= 4)
                {
                  def.value1 = ( *d_iter )[ 3 ];
                }
                if ( len >= 5)
                {
                  def.value2 = ( *d_iter )[ 4 ];
                }
                cfg1_def_table_.push_back( def );
              }
            }
          }
        }
        cfg1_out::cfg1_def_table cfg1_def_table_;
        bool without_def_table_;
      };
      static init_t init;
      cfg1_out::cfg1_def_table const* result = init.without_def_table_ ? 0 : &init.cfg1_def_table_;
      return result;
    }

    //! オブジェクトの交換
    void factory::do_swap( factory& other )
    {
      kernel_.swap( other.kernel_ );
    }

    /*!
     *  \brief  マクロプロセッサの生成
     *  \param[in]  cfg1out cfg1_out オブジェクト
     *  \param[in]  api_map .cfg ファイルに記述された静的API情報
     *  \return     マクロプロセッサへのポインタ
     *  \note   このメンバ関数は従来仕様（ソフトウェア部品非対応版）の温存のためにそのまま残す。
     */
    std::unique_ptr< macro_processor > factory::do_create_macro_processor( cfg1_out const& cfg1out, cfg1_out::static_api_map const& api_map ) const
    {
      typedef macro_processor::element element;
      typedef macro_processor::var_t var_t;
      std::unique_ptr< macro_processor > mproc( new macro_processor );
      element e;

      e.s = " ";    mproc->set_var( "SPC", var_t( 1, e ) );  // $SPC$
      e.s = "\t";   mproc->set_var( "TAB", var_t( 1, e ) );  // $TAB$
      e.s = "\n";   mproc->set_var( "NL",  var_t( 1, e ) );  // $NL$

      // バージョン情報
      e.s = toppers::get_global_string( "version" );
      e.i = toppers::get_global< std::tr1::int64_t >( "timestamp" );
      mproc->set_var( "CFG_VERSION", var_t( 1, e ) );   // $CFG_VERSION$

      // その他の組み込み変数の設定
      set_object_vars( api_map, *mproc );
      set_object_vars( cfg1out.get_static_api_array(), *mproc );
      set_clsid_vars( cfg1out.get_clsid_table(), cfg1out, *mproc );
      set_domid_vars( cfg1out.get_domid_table(), *mproc );
      set_platform_vars( cfg1out, *mproc );
      e.s = cfg1out.get_includes();
      e.i = boost::none;
      mproc->set_var( "INCLUDES", var_t( 1, e ) );

      // パス情報
      e.s = boost::lexical_cast< std::string >(toppers::get_global< int >( "pass" ));
      e.i = toppers::get_global< int >( "pass" );
      mproc->set_var( "CFG_PASS", var_t( 1, e ) );

      return mproc;
    }

    /*!
     *  \brief  マクロプロセッサの生成
     *  \param[in]  cfg1out cfg1_out オブジェクト
     *  \param[in]  api_array .cfg ファイルに記述された静的API情報
     *  \return     マクロプロセッサへのポインタ
     */
    std::unique_ptr< macro_processor > factory::do_create_macro_processor( cfg1_out const& cfg1out, std::vector< static_api > const& api_array ) const
    {
      typedef macro_processor::element element;
      typedef macro_processor::var_t var_t;
      std::unique_ptr< macro_processor > mproc( new macro_processor );
      element e;

      e.s = " ";    mproc->set_var( "SPC", var_t( 1, e ) );  // $SPC$
      e.s = "\t";   mproc->set_var( "TAB", var_t( 1, e ) );  // $TAB$
      e.s = "\n";   mproc->set_var( "NL",  var_t( 1, e ) );  // $NL$

      // バージョン情報
      e.s = toppers::get_global_string( "version" );
      e.i = toppers::get_global< std::tr1::int64_t >( "timestamp" );
      mproc->set_var( "CFG_VERSION", var_t( 1, e ) );   // $CFG_VERSION$

      // その他の組み込み変数の設定
      set_object_vars( api_array, *mproc );
      set_clsid_vars( cfg1out.get_clsid_table(), cfg1out, *mproc );
      set_domid_vars( cfg1out.get_domid_table(), *mproc );
      set_platform_vars( cfg1out, *mproc );
      e.s = cfg1out.get_includes();
      e.i = boost::none;
      mproc->set_var( "INCLUDES", var_t( 1, e ) );

      // パス情報
      e.s = boost::lexical_cast< std::string >( toppers::get_global< int >( "pass" ) );
      e.i = toppers::get_global< int >( "pass" );
      mproc->set_var( "CFG_PASS", var_t( 1, e ) );

      return mproc;
    }

    std::unique_ptr< cfg1_out > factory::do_create_cfg1_out( std::string const& filename ) const
    {
      return std::unique_ptr< itronx::cfg1_out >( new cfg1_out( filename, get_cfg1_def_table() ) );
    }

    std::unique_ptr< checker > factory::do_create_checker() const
    {
      return std::unique_ptr< itronx::checker >( new checker );
    }

  }
}

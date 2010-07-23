/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007-2010 by TAKAGI Nobuhisa
 * 
 *  ��L���쌠�҂́C�ȉ���(1)�`(4)�̏����𖞂����ꍇ�Ɍ���C�{�\�t�g�E�F
 *  �A�i�{�\�t�g�E�F�A�����ς������̂��܂ށD�ȉ������j���g�p�E�����E��
 *  �ρE�Ĕz�z�i�ȉ��C���p�ƌĂԁj���邱�Ƃ𖳏��ŋ�������D
 *  (1) �{�\�t�g�E�F�A���\�[�X�R�[�h�̌`�ŗ��p����ꍇ�ɂ́C��L�̒���
 *      ���\���C���̗��p��������щ��L�̖��ۏ؋K�肪�C���̂܂܂̌`�Ń\�[
 *      �X�R�[�h���Ɋ܂܂�Ă��邱�ƁD
 *  (2) �{�\�t�g�E�F�A���C���C�u�����`���ȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł���`�ōĔz�z����ꍇ�ɂ́C�Ĕz�z�ɔ����h�L�������g�i���p
 *      �҃}�j���A���Ȃǁj�ɁC��L�̒��쌠�\���C���̗��p��������щ��L
 *      �̖��ۏ؋K����f�ڂ��邱�ƁD
 *  (3) �{�\�t�g�E�F�A���C�@��ɑg�ݍ��ނȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł��Ȃ��`�ōĔz�z����ꍇ�ɂ́C���̂����ꂩ�̏����𖞂�����
 *      �ƁD
 *    (a) �Ĕz�z�ɔ����h�L�������g�i���p�҃}�j���A���Ȃǁj�ɁC��L�̒�
 *        �쌠�\���C���̗��p��������щ��L�̖��ۏ؋K����f�ڂ��邱�ƁD
 *    (b) �Ĕz�z�̌`�Ԃ��C�ʂɒ�߂���@�ɂ���āCTOPPERS�v���W�F�N�g��
 *        �񍐂��邱�ƁD
 *  (4) �{�\�t�g�E�F�A�̗��p�ɂ�蒼�ړI�܂��͊ԐړI�ɐ����邢���Ȃ鑹
 *      �Q������C��L���쌠�҂����TOPPERS�v���W�F�N�g��Ɛӂ��邱�ƁD
 *      �܂��C�{�\�t�g�E�F�A�̃��[�U�܂��̓G���h���[�U����̂����Ȃ闝
 *      �R�Ɋ�Â�����������C��L���쌠�҂����TOPPERS�v���W�F�N�g��
 *      �Ɛӂ��邱�ƁD
 * 
 *  �{�\�t�g�E�F�A�́C���ۏ؂Œ񋟂���Ă�����̂ł���D��L���쌠�҂�
 *  ���TOPPERS�v���W�F�N�g�́C�{�\�t�g�E�F�A�Ɋւ��āC����̎g�p�ړI
 *  �ɑ΂���K�������܂߂āC�����Ȃ�ۏ؂��s��Ȃ��D�܂��C�{�\�t�g�E�F
 *  �A�̗��p�ɂ�蒼�ړI�܂��͊ԐړI�ɐ����������Ȃ鑹�Q�Ɋւ��Ă��C��
 *  �̐ӔC�𕉂�Ȃ��D
 * 
 */
#include <cctype>
#include <sstream>
#include "toppers/diagnostics.hpp"
#include "toppers/itronx/static_api.hpp"
#include "toppers/itronx/static_api_parser.hpp"
#include <boost/spirit/include/classic.hpp>
#include <boost/lexical_cast.hpp>

namespace toppers
{
  namespace itronx
  {
    namespace
    {
      class find_api_predicate
      {
      public:
        explicit find_api_predicate( std::string const& api_name ) : api_name_( api_name ) {}
        bool operator()( static_api::info const& info ) const
        {
          return info.api_name == api_name_;
        }
      private:
        std::string api_name_;
      };
    }

    /*!
     *  \brief  �ÓIAPI�̍\�����
     *  \param[in,out]  next      �e�L�X�g�̓ǂݍ��݈ʒu�A���\����͌�̎��̓ǂݍ��݈ʒu�̊i�[��
     *  \param[in]      last      �e�L�X�g�̏I�[�ʒu
     *  \param[in]      info_map  �ÓIAPI���}�b�v
     *  \param[in]      ucn       ���ە�������L���ɂ���ꍇ�� true ���w�肷��B
     *  \param[in]      codeset   �����R�[�h
     *  \retval         true    ����
     *  \retval         false   ���s
     */
    bool static_api::parse( text::const_iterator& next, text::const_iterator last,
                            std::map< std::string, info > const& info_map,
                            bool ucn, codeset_t codeset )
    {
      boost::spirit::classic::parse_info< text::const_iterator > pi;
      std::vector< std::string > tokens;
      c_const_expr_parser cexpr_p( ucn, codeset );
      static_api_parser parser( tokens, cexpr_p );
      static_api temp;

      text::const_iterator next_temp;
      for ( next_temp = next; next_temp != last; ++next_temp )
      {
        if ( !std::isspace( static_cast< unsigned char >( *next_temp ) ) )
        {
          break;
        }
      }
      if ( next_temp == last )
      {
        return false;
      }

      // �G���[���b�Z�[�W�p�̍s�ԍ�
      // ���̍s�ԍ��́A�ÓIAPI�̊J�n�ʒu�̂��̂ł��邽�߁A�G���[�����ӏ����̂��̃Y�o�����w�����Ƃ͂ł��Ȃ��B
      temp.line_ = next_temp.line();

      pi = boost::spirit::classic::parse( next_temp, last, parser, boost::spirit::classic::space_p );
      if ( !pi.hit )
      {
        return false;
      }

      // �ÓIAPI�����݂��邩�ǂ����̔��肨��уV�O�j�`���̎擾
      std::string api_name( tokens.front() );
      std::map< std::string, info >::const_iterator info_iter = info_map.find( api_name );
      info const* pinfo = 0;
      if ( info_iter != info_map.end() )
      {
        pinfo = &info_iter->second;
        temp.pinfo_ = pinfo;
      }
      else
      {
        error( temp.line_, _( "static API `%1%\' is unknown" ), api_name );
        return false;
      }

      // �e�p�����[�^�̉��
      std::istringstream iss( pinfo->params );
      int order = 0;            // �p�����[�^���X�g���̏���
      bool param_list = false;  // �p�����[�^���X�g��͒��t���O
      std::string symbol;
      bool skip = false;

      for ( std::vector< std::string >::const_iterator iter( tokens.begin() + 1 ), last( tokens.end() );
            iter != last;
            ++iter )
      {
        if ( !param_list )
        {
          iss >> symbol;
          if ( symbol.size() > 3 && symbol.substr( symbol.size() - 3 ) == "..." )
          {
            param_list = true;
            order = 0;
            symbol.resize( symbol.size() - 3 );
          }
        }
        if ( symbol == "{" || symbol == "}" )
        {
          if ( symbol != *iter )
          {
            error( temp.line_, _( "missing token `%1%\'" ), symbol );
            --iter;
          }
        }
        else if ( *iter == "{" || *iter == "}" )
        {
          if ( param_list && *iter == "}" )
          {
            iss >> symbol;
            if ( symbol != "}" )
            {
              error( temp.line_, _( "illegal token `%1%\'" ), *iter );
            }
            param_list = false;
          }
          else if ( *symbol.rbegin() == '\?' ) // �ȗ��\�p�����[�^�̃X�L�b�v
          {
            skip = true;
          }
          else
          {
            error( temp.line_, _( "illegal token `%1%\'" ), *iter );
          }
        }
        else
        {
          parameter value;
          value.symbol = symbol;
          if ( param_list )
          {
            value.symbol += boost::lexical_cast< std::string >( order++ );
          }
          value.text = *iter;
          value.value = 0;
          temp.params_.push_back( value );
        }
      }
      if ( !iss.eof() )
      {
        iss >> symbol;
        if ( symbol != "}" )
        {
          error( temp.line_, _( "few parameters for static API `%s\'" ), api_name );
        }
        else if ( !skip )
        {
          error( temp.line_, _( "missing token `%1%\'" ), symbol );
        }
        else
        {
          skip = false;
        }
      }
      next = pi.stop;
      swap( temp );
      return true;
    }

    bool static_api::set_block( char const* type, std::string const& id )
    {
      for ( std::vector< parameter >::const_iterator iter( params_.begin() ), last( params_.end() );
            iter != last;
            ++iter )
      {
        if ( iter->symbol == type )
        {
          return false;
        }
      }
      parameter value;
      value.symbol = type;
      value.text = id;
      value.value = 0;
      params_.push_back( value );
      return true;
    }

    static_api::size_type static_api::count_integer_params() const
    {
      size_type result = 0;
      for ( std::vector< parameter >::const_iterator iter( params_.begin() ), last( params_.end() );
            iter != last;
            ++iter )
      {
        if ( !iter->symbol.empty()
          && ( ( iter->symbol[0] == '.' ) || ( iter->symbol[0] == '+' ) ) )
        {
          ++result;
        }
      }
      return result;
    }

  }
}

/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007-2008 by TAKAGI Nobuhisa
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
/*!
 *  \file   toppers/itronx/static_api.hpp
 *  \brief  �ÓIAPI���̉�͂ƊǗ��Ɋւ���錾��`
 *
 *  ���̃t�@�C���Œ�`�����N���X
 *  \code
 *  class static_api;
 *  \endcode
 */
#ifndef TOPPERS_ITRONX_STATIC_API_HPP_
#define TOPPERS_ITRONX_STATIC_API_HPP_

#include <string>
#include <map>
#include <set>
#include <algorithm>
#include "toppers/workaround.hpp"
#include "toppers/text.hpp"
#include "toppers/codeset.hpp"
#include "toppers/diagnostics.hpp"
#include <boost/any.hpp>
#include <boost/optional.hpp>

namespace toppers
{
  namespace itronx
  {

    /*!
     *  \class  static_api static_api.hpp "toppers/itronx/static_api.hpp"
     *  \brief  �R���t�B�M�����[�V�����t�@�C���ɋL�q���ꂽ�ÓIAPI�̏����Ǘ����邽�߂̃N���X
     */
    class static_api
    {
    public:
      /*!
       *  \struct info  static_api.hpp "toppers/itronx/static_api.hpp"
       *  \brief  �ÓIAPI�̎d�l�Ɋւ�����
       *
       *  static_api::info �� params �����o���̊e�v�f�ɕt����ꂽ�ړ����̈Ӗ��͎��̂Ƃ���
       *
       *  - #: �I�u�W�F�N�g���ʎq
       *  - %: �I�u�W�F�N�g���ʎq�ŁA���������t���̑ΏۂƂȂ�Ȃ�����
       *  - .: �������������萔���p�����[�^
       *  - +: �����t�������萔���p�����[�^
       *  - $: ������萔���p�����[�^
       *  - &: ��ʒ萔���p�����[�^
       *
       *  �Ȃ��A{ ����� } ���֋X�I�Ƀp�����[�^�̈��Ƃ��Ĉ����Ă���i�\����͂ɕK�v�Ȃ��߁j�B
       */
      struct info
      {
        char const* type;     //!< �J�[�l���I�u�W�F�N�g�̎�ʂ�\���O�����i"tsk", "sem"���j
        char const* api_name; //!< �ÓIAPI���i"CRE_TSK", "CRE_SEM"���j
        char const* params;   //!< �p�����[�^����
        int id_pos;           //!< �p�����[�^�̂���ID�ԍ��̈ʒu�i-1�̏ꍇ��ID�ԍ������j
        bool slave;           //!< ���̐ÓIAPI�ɑ΂��ď]���֌W�ɂ���ꍇ��true�i"DEF_TEX"���j
      };
      /*!
       *  \struct parameter static_api.hpp "toppers/itronx/static_api.hpp"
       *  \brief  �ÓIAPI�̃p�����[�^�Ɋւ�����
       */
      struct parameter
      {
        std::string symbol;   //!< ��������
        std::string text;     //!< �������̎���
        boost::optional< std::tr1::intmax_t > value;  //!< �������̒l
        std::string string;
      };
      typedef std::vector< parameter > parameter_container;
      typedef parameter_container::iterator iterator;
      typedef parameter_container::const_iterator const_iterator;
      typedef parameter_container::reference reference;
      typedef parameter_container::const_reference const_reference;
      typedef parameter_container::size_type size_type;

      char const* api_name() const { return pinfo_ != 0 ? pinfo_->api_name : ""; }
      info const* get_info() const { return pinfo_; }

      parameter_container const& params() const { return params_; }
      iterator begin() { return params_.begin(); }
      const_iterator begin() const { return params_.begin(); }
      iterator end() { return params_.end(); }
      const_iterator end() const { return params_.end(); }
      reference at( size_type pos ) { return params_.at( pos ); }
      reference at( std::string const& symbol )
      {
        iterator iter = std::find_if( params_.begin(), params_.end(), match_param_symbol( symbol ) );
        if ( iter == params_.end() )
        {
          throw std::out_of_range( "out of range" );
        }
        return *iter;
      }
      const_reference at( size_type pos ) const { return params_.at( pos ); }
      const_reference at( std::string const& symbol ) const
      {
        const_iterator iter = std::find_if( params_.begin(), params_.end(), match_param_symbol( symbol ) );
        if ( iter == params_.end() )
        {
          throw std::out_of_range( "out of range" );
        }
        return *iter;
      }
      parameter id() const
      {
        if ( pinfo_->id_pos < 0 )
        {
          return parameter();
        }
        return at( pinfo_->id_pos );
      }
      
      text_line const& line() const { return line_; }
      void line( text_line const& value ) { line_ = value; }
      size_type count_integer_params() const;
        
      //! �I�u�W�F�N�g�̌���
      void swap( static_api& other )
      {
        std::swap( pinfo_, other.pinfo_ );
        params_.swap( other.params_ );
        line_.swap( other.line_ );
      }

      bool parse( text::const_iterator& next, text::const_iterator last, 
                  std::map< std::string, info > const& info_map,
                  bool ucn = false, codeset_t codeset = ascii );

      bool set_class( std::string const& id )
      {
        return set_block( "*CLASS", id );
      }
      bool set_domain( std::string const& id )
      {
        return set_block( "*DOMAIN", id );
      }

      /*!
       *  \brief  ID�ԍ��̊��t��
       *  \param[in]  first   ID�ԍ�������t���� static_api ��̐擪�ʒu
       *  \param[in]  last    ID�ԍ�������t���� static_api ��̏I�[ + 1
       */
      template < class ForwardIterator >
        static void assign_id( ForwardIterator first, ForwardIterator last )
      {
        std::map< std::string, long > id_map;
        std::map< std::string, std::set< std::string > > slave_id_set;
        std::vector< std::string > id_res;
        typedef std::vector< std::string >::size_type size_type;

        // �\��ς݂�ID�ԍ���􂢏o��
        for ( ForwardIterator iter( first ); iter != last; ++iter )
        {
          static_api::info const* info = iter->get_info();
          if ( info->id_pos >= 0 )
          {
            if ( !info->slave )
            {
              if ( iter->at( info->id_pos ).symbol[0] == '#' )
              {
                boost::optional< std::tr1::int64_t > id_value = *iter->at( info->id_pos ).value;
                if ( id_value )
                {
                  long id = static_cast< long >( *id_value );
                  if ( id > 0 )
                  {
                    long n = static_cast< long >( id_res.size() );
                    if ( n < id + 1 )
                    {
                      n = id + 1;
                    }
                    id_res.resize( n );
                    std::string name( iter->at( info->id_pos ).text );
                    if ( !id_res[ id ].empty() )
                    {
                      fatal( _( "%1% `%2%\' in %3% is duplicated" ), iter->at( info->id_pos ).symbol.c_str() + 1, id, info->api_name );
                    }
                    id_res[ id ] = name;
                  }
                }
              }
            }
          }
        }

        // �\�񂳂�Ă��Ȃ�ID�ԍ����������t��
        long id = 1;
        for ( ForwardIterator iter( first ); iter != last; ++iter )
        {
          static_api::info const* info = iter->get_info();
          if ( info->id_pos >= 0 )
          {
            std::string name( iter->at( info->id_pos ).text );
            if ( !info->slave )
            {
              long id_value = -1;
              if ( iter->at( info->id_pos ).symbol[0] == '#' )
              {
                std::vector< std::string >::iterator id_iter( std::find( id_res.begin(), id_res.end(), name ) );
                if ( id_iter != id_res.end() )  // ����t���ς݂̏ꍇ...
                {
                  id_value = id_iter - id_res.begin();
                }
                else  // �܂�����t�����Ă��Ȃ��ꍇ...
                {
                  long n = static_cast< long >( id_res.size() );
                  while ( id < n && !id_res[ id ].empty() )
                  {
                    ++id;
                  }
                  if ( n < id + 1 )
                  {
                    n = id + 1;
                  }
                  id_res.resize( n );
                  id_res[ id ] = name;
                  id_value = id;
                  iter->at( info->id_pos ).value = id_value;
                }
                if ( id_map.find( name ) != id_map.end() )
                {
                  fatal( iter->line(), _( "E_OBJ: `%1%\' is duplicated" ), name );
                }
              }
              else if ( iter->at( info->id_pos ).value )
              {
                id_value = static_cast< long >( iter->at( info->id_pos ).value.get() );
              }
              id_map[ name ] = id_value;
            }
            else  // slave
            {
              if ( id_map[ name ] < 1 )
              {
                fatal( iter->line(), _( "`%1%\' is undefined" ), iter->at( info->id_pos ).text );
              }
              std::set< std::string >& set = slave_id_set[ info->api_name ];
              if ( set.find( name ) != set.end() )  // DEF_TEX�d����`�̔���
              {
                fatal( iter->line(), _( "E_OBJ: `%1%\' is duplicated" ), iter->at( info->id_pos ).text );
              }
              set.insert( name );
            }
          }
        }

        if ( !id_res.empty() && std::find( id_res.begin() + 1, id_res.end(), std::string() ) != id_res.end() )
        {
          error( _( "`%1%\' id numbers do not continue" ), first->get_info()->type );          
        }
      }
    private:
      class match_param_symbol
      {
      public:
        explicit match_param_symbol( std::string const& symbol ) : symbol_( symbol ) {}
        bool operator()( parameter const& param ) const
        {
          return param.symbol == symbol_;
        }
      private:
        std::string symbol_;
      };

      bool set_block( char const* type, std::string const& id );

      info const* pinfo_;
      std::vector< parameter > params_;
      text_line line_;
    };

  }
}

#endif  // ! TOPPERS_ITRONX_STATIC_API_HPP_

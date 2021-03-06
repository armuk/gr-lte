# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
install(FILES
    lte_rate_unmatch_vff.xml
    lte_descrambling_vfvf.xml
    lte_viterbi_vfvb.xml
    lte_vector_resize_vbvb.xml
    lte_block_tests.xml
    lte_crc_calculator_vbvb.xml
    lte_mib_unpack_vb.xml
    lte_qpsk_soft_demod_vcvf.xml
    lte_layer_demapper_vcvc.xml
    lte_pre_decoder_vcvc.xml
    lte_pbch_demux_vcvc.xml
    lte_extract_occupied_tones_vcvc.xml
    lte_remove_cp_cvc.xml
    lte_tag_symbol_cc.xml
    lte_hier_freq_estimate_cc.xml
    lte_freq_estimate_c.xml
    lte_hier_sss_sync_cc.xml
    lte_sss_selector_cvc.xml
    lte_sss_calc_vc.xml
    lte_sss_tagging_cc.xml
    lte_pss_selector_cvc.xml
    lte_pss_calc_vc.xml
    lte_pss_tagging_cc.xml
    lte_hier_pss_sync_cc.xml
    lte_cp_time_freq_sync_cc.xml
    lte_zadoff_chu_seq_generator_vc.xml
    lte_decode_bch_vfvb.xml
    lte_decode_pbch_vcvf.xml
    lte_channel_estimator_vcvc.xml
    lte_estimator_parameterizer_msg.xml
    lte_pcfich_descrambler_vfvf.xml
    lte_cfi_unpack_vf.xml
    lte_pcfich_demux_vcvc.xml
    lte_decode_pcfich_vcm.xml DESTINATION share/gnuradio/grc/blocks
)

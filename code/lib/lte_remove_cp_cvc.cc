/* -*- c++ -*- */
/*
 * Copyright 2012 Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT)
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <lte_remove_cp_cvc.h>
#include <cstdio>


lte_remove_cp_cvc_sptr
lte_make_remove_cp_cvc (int fftl, std::string key)
{
	return lte_remove_cp_cvc_sptr (new lte_remove_cp_cvc (fftl, key));
}


lte_remove_cp_cvc::lte_remove_cp_cvc (int fftl, std::string key)
	: gr_block ("remove_cp_cvc",
		gr_make_io_signature (1,1, sizeof (gr_complex)),
		gr_make_io_signature (1,1, sizeof (gr_complex)*fftl)),
		d_fftl(fftl),
		d_cpl((144*fftl)/2048),
		d_cpl0((160*fftl)/2048),
		d_slotl(7*fftl+6*d_cpl+d_cpl0),
		d_symb(0),
		d_sym_num(0),
		d_work_call(0),
		d_found_frame_start(false),
		d_frame_start(0)
{
    d_key=pmt::pmt_string_to_symbol(key);
	d_tag_id=pmt::pmt_string_to_symbol(name() );
	set_tag_propagation_policy(TPP_DONT);
}


lte_remove_cp_cvc::~lte_remove_cp_cvc ()
{
}

void
lte_remove_cp_cvc::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = ( d_fftl + d_cpl0 ) * noutput_items;
}

int
lte_remove_cp_cvc::general_work (int noutput_items,
			       gr_vector_int &ninput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];
    gr_complex *out = (gr_complex *) output_items[0];

    d_work_call++;

    // the following section removes the samples before the first frame start.
    std::vector <gr_tag_t> v;
    get_tags_in_range(v,0,nitems_read(0),nitems_read(0)+noutput_items*(d_fftl+d_cpl0) );
    int size = v.size();
    if(!d_found_frame_start){
        for(int i = 0 ; i < size ; i++ ){
            long value = pmt::pmt_to_long(v[i].value);
            if(value == 0){
                //std::string key = pmt::pmt_symbol_to_string(v[0].key);
                //printf("%s found frame start offset = %li\trange = %i\tkey = %s\n",name().c_str(), v[0].offset,noutput_items*(fftl+cpl0),key.c_str() );
                int delay = int(v[i].offset-nitems_read(0) );
                d_frame_start = (v[i].offset);
                printf("\n%s\n",name().c_str() );
                //printf("nitems_read = %li\n",nitems_read(0) );
                //printf("delay       = %i\n",delay);
                //printf("a+b         = %li\n",nitems_read(0)+delay );
                printf("frame_start = %ld\n",d_frame_start);
                d_frame_start = d_frame_start%(20*d_slotl);
                printf("mod start   = %ld\n\n",d_frame_start);
                d_sym_num = 0;
                d_symb = 0;
                d_found_frame_start = true;
                consume_each(delay);
                return 0;
            }
            else{
                if(size > i+1){
                    continue;
                }
                else{
                    consume_each(noutput_items*(d_fftl+d_cpl0));
                    return 0;
                }
            }
        }

        if(size == 0){
            consume_each(noutput_items*(d_fftl+d_cpl0));
            return 0;
        }
    }
    for(int i = 0 ; i < size ; i++ ){
        if(size > 0 && pmt::pmt_to_long(v[i].value) == 0 ){
            if( (v[i].offset)%(20*d_slotl) != d_frame_start ){
                printf("%s OUT of sync!\n", name().c_str() );
                d_found_frame_start = false;
                return 0;
            }
        }
    }


    // Copy the samples of interest from input to output buffer
    long consumed_items = copy_samples_from_in_to_out(out, in, noutput_items);

    // add item tags. Item tags for each vector/OFDM symbol.
    add_tags_to_vectors(noutput_items);

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (consumed_items);
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

inline int
lte_remove_cp_cvc::copy_samples_from_in_to_out(gr_complex* out, const gr_complex* in, int noutput_items)
{
    int consumed_items = 0;
    int vector_byte_size = sizeof(gr_complex)*d_fftl;
    int syml0 = d_cpl0 + d_fftl;
    int syml1 = d_cpl + d_fftl;

    for (int i = 0 ; i < noutput_items ; i++){
        if(d_symb == 0){
            memcpy(out, in+d_cpl0, vector_byte_size);
            consumed_items += syml0;
            in += syml0;
        }
        else{
            memcpy(out, in+d_cpl,vector_byte_size);
            consumed_items += syml1;
            in += syml1;
        }
        out += d_fftl;
        d_symb =(d_symb+1)%7;
    }
    return consumed_items;
}

inline void
lte_remove_cp_cvc::add_tags_to_vectors(int noutput_items)
{
    for (int i = 0 ; i < noutput_items ; i++){
        if(d_sym_num%7 == 0){
            add_item_tag(0,nitems_written(0)+i,d_key, pmt::pmt_from_long(d_sym_num),d_tag_id);
        }
		d_sym_num=(d_sym_num+1)%140;
	}
}




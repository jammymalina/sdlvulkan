
set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab

set colorcolumn=110
highlight ColorColumn ctermbg=darkgray

augroup project
    autocmd!
    autocmd BufRead,BufNewFile *.h,*.c set filetype=c.doxygen
augroup END

let g:syntastic_c_compiler="gcc"
let g:syntastic_check_on_open=1
let g:syntastic_check_on_wq=0
let g:syntastic_error_symbol="✗"
let g:syntastic_c_compiler_options="-std=c11"
let g:syntastic_c_include_dirs=[]

let NERDTreeMinimalUI=1
let NERDTreeDirArrows=1

let g:ycm_global_ycm_extra_conf = "./.ycm_extra_conf.py"


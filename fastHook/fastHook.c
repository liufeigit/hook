// Copyright (c) 2014, 部落邦 , Inc.
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name Facebook nor the names of its contributors may be used to
//     endorse or promote products derived from this software without specific
//     prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// author @刘飞

#include <objc/runtime.h>
#include <mach-o/dyld.h>
#include <mach-o/nlist.h>
#include <mach-o/loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>


typedef struct mach_header_64 header;
typedef struct load_command command;
typedef struct segment_command_64 segment;
typedef struct symtab_command symtab;
typedef struct dysymtab_command dysymtab;
typedef struct section_64 section;
// typedef struct nlist_64 nlist;

// 三张基本表
typedef struct  LaSymPtrMeta{
	int offset; // 当前偏移
	int size; // 大小
	int index; // symbol索引指向DySymInd
	void *base; // 基址指针
} LaSymPtrMeta;

typedef struct DyIndSymMeta{
	int offset;
	int count;
	void *base;
} DyIndSymMeta;

typedef struct SymTabMeta{
	int offset;
	int count; // 符号个数
	void *base; // 基址指针
} SymTabMeta;

typedef struct StrTabMeta{
	int offset;
	int size;
	void *base; // 基址指针
} StrTabMeta;

typedef struct RelationTabMeta{
	LaSymPtrMeta laSymPtrMeta;
	DyIndSymMeta dyIndSymMeta;
	StrTabMeta strTabMeta;
	SymTabMeta symTabMeta;
	void *base;
	
} RelationTabMeta;

typedef struct SymMeta{
	char *name;
	int type; // "stab" "ext" "local" "other" ???
	int symTabIndex; // -1为不存在
	int dyIndSymIndex; // -1为不存在
	int laSymPtrIndex; // -1为不存在
	int _flag; // 匹配标志
} SymMeta;

// 获取镜像的关联符号表格
void getRelationTabMeta(int num,RelationTabMeta *relationTabMeta){

	LaSymPtrMeta *laSymPtrMeta;
	DyIndSymMeta *dyIndSymMeta;
	SymTabMeta *symTabMeta;
	StrTabMeta *strTabMeta;

	laSymPtrMeta=&(relationTabMeta->laSymPtrMeta);
	dyIndSymMeta=&(relationTabMeta->dyIndSymMeta);
	symTabMeta=&(relationTabMeta->symTabMeta);
	strTabMeta=&(relationTabMeta->strTabMeta);

	header *_header; // mach_header_64头
	char *base; // 基址指针
	char *curBase; // 当前指针
	char *curBaseSect; // 当前section指针

	int ncmds; 
	int cmd;
	int cmdSize;
	int nsects;

	command *curCom;
	segment *curSeg;
	section *curSect;
	char segName[16];

	_header=_dyld_get_image_header(num);
	base=(char *)_header;
	ncmds=_header->ncmds;
	curBase=base+sizeof(header);

	for(int i=0;i<ncmds;i++){
		curCom=(command *)curBase; // 获取当前命令
		cmd=curCom->cmd;
		cmdSize=curCom->cmdsize;

		// 命令分类
		switch(cmd){
			case LC_SEGMENT_64:

				{
					char sectName[24];
					int size;
					int offset;
					int reserved1;
					int reserved2;

					curSeg=(segment *)curBase;
					strcpy(segName, curSeg->segname);
					nsects=curSeg->nsects;

					if(strcmp(segName, SEG_TEXT)==0){
						;;
					}
					else if(strcmp(segName, SEG_DATA)==0){
						curBaseSect=curBase+sizeof(segment); // 获取当前section指针

						for(int i=0;i<nsects;i++){
							
							curSect=(section *)curBaseSect;
							strcpy(sectName, curSect->sectname);
							size=curSect->size;
							offset=curSect->offset;
							reserved1=curSect->reserved1;
							reserved2=curSect->reserved2;

							if(strcmp(sectName, "__la_symbol_ptr")==0){
								// offset , size, index
								laSymPtrMeta->offset=offset;
								laSymPtrMeta->size=size;
								laSymPtrMeta->index=reserved1;
								laSymPtrMeta->base=base;
							}
							else{
								;;
							}
							curBaseSect=curBaseSect+sizeof(section);

						}

					}
					else{
						;;
					}

				}
				;;
				break;
			case LC_SYMTAB:
				{
					symtab *curTab;
					int symOff;
					int strOff;
					int nSyms;
					int strSize;

					curTab=(symtab *)curBase;
					symOff=curTab->symoff;
					strOff=curTab->stroff;
					nSyms=curTab->nsyms;
					strSize=curTab->strsize;

					symTabMeta->base=base;
					symTabMeta->count=nSyms;
					symTabMeta->offset=symOff;

					strTabMeta->base=base;
					strTabMeta->offset=strOff;
					strTabMeta->size=strSize;

					relationTabMeta->base=base;
				}
				;;
				break;
			case LC_DYSYMTAB:
				{
					dysymtab *curTab;
					int indirectSymOff;
					int nIndirectSyms;

					curTab=(dysymtab *)curBase;
					indirectSymOff=curTab->indirectsymoff;
					nIndirectSyms=curTab->nindirectsyms;

					dyIndSymMeta->base=base;
					dyIndSymMeta->count=nIndirectSyms;
					dyIndSymMeta->offset=indirectSymOff;

				}
				;;
				break;
			default:

				;;
		}

		curBase=curBase+cmdSize; // 移动当前指针

		// printf("curBase is %p",curBase);

	}

	// printf("......end");

}

SymMeta *findSymMeta(int num,char *name){

	LaSymPtrMeta laSymPtrMeta;
	DyIndSymMeta dyIndSymMeta;
	SymTabMeta symTabMeta;
	StrTabMeta strTabMeta;
	RelationTabMeta relationTabMeta;

	struct nlist_64 *symTabBase;
	char *laSymPtrBase;
	char *dyIndSymBase;
	char *strTabBase;
	char *base;
	SymMeta *symMeta;

	int symCount;
	int dyIndSymCount;
	int laSymSize;
	int laSymPtrIndex;
	int dyIndSymIndex;
	int symTabIndex;

	symMeta=(SymMeta *)malloc(sizeof(SymMeta)*1);
	symMeta->_flag=0;
	getRelationTabMeta(num,&relationTabMeta);
	laSymPtrMeta=relationTabMeta.laSymPtrMeta;
	dyIndSymMeta=relationTabMeta.dyIndSymMeta;
	symTabMeta=relationTabMeta.symTabMeta;
	strTabMeta=relationTabMeta.strTabMeta;

	symCount=symTabMeta.count;
	dyIndSymCount=dyIndSymMeta.count;
	laSymPtrIndex=laSymPtrMeta.index;

	base=(char *)relationTabMeta.base;
	symTabBase=(struct nlist_64 *)(base+symTabMeta.offset);
	laSymPtrBase=base+laSymPtrMeta.offset;
	dyIndSymBase=base+dyIndSymMeta.offset;
	strTabBase=base+strTabMeta.offset;

	int nIndex;
	char nType;
	char nSect;

	for(int i=0;i<symCount;i++){
		int _stab=0xe0;
		int _ext=0x01;
		int _undf=0x0;
		int _sect=0xe;
		int _type,_type1;

		// 过滤stab调试列表
		nType=symTabBase->n_type;
		_type=nType|_stab;
		// ??? 白名单机制 预防出错 
		// ??? 2014/8/22 21:30 掩码问题和其它情况
		if(nType==_ext){ //  _ext|_undf外部链接符号 2014/8/22 19:15 优先处理
			int strIndex=symTabBase->n_un.n_strx; // 取得当前符号的strTab索引
			char *curStr=strTabBase+strIndex+1; // "."界定符
			if(strcmp(curStr, name)==0){ // 如果匹配上的话

				int _num,_bs;
				char *_base;
				int _index,_flag;

				_flag=0;
				symTabIndex=i; // symTab中的索引
				_num=dyIndSymCount-laSymPtrIndex;
				_bs=(strTabMeta.offset-dyIndSymMeta.offset)/dyIndSymCount;

				for(int i=0;i<_num;i++){
					int *_ptr;

					_base=dyIndSymBase+(i+laSymPtrIndex)*_bs;
					_ptr=(int *)_base;
					_index=*_ptr;

					if(_index==symTabIndex){ // 从laSymPtr匹配成功
						// 初始化symMeta信息
						dyIndSymIndex=i+laSymPtrIndex;
						laSymPtrIndex=i;

						symMeta->_flag=1;
						symMeta->dyIndSymIndex=dyIndSymIndex;
						symMeta->laSymPtrIndex=laSymPtrIndex;
						symMeta->symTabIndex=symTabIndex;
						symMeta->name=name;
						symMeta->type=1;

						break;

					}

				}

				if(symMeta->_flag==1){ // 匹配成功
					break;
				}
			}
		}	
		else{

		}

		symTabBase=symTabBase+1;
	}

 	// 返回值断定
	if(symMeta->_flag==1){
		return symMeta;
	}
	else{
		free(symMeta);
		return NULL;
	}

}

int setLaSymPtr(int num,char *name,void *fn){

	int flag;
	RelationTabMeta relationTabMeta;
	int laIndex;
	char *base;
	char *curLaSymBase;
	int offset;
	int size;
	SymMeta *symMeta;

	flag=0;
	getRelationTabMeta(num,&relationTabMeta);
	symMeta=findSymMeta(num,name);

	if(symMeta){
		int _size;
		int **_ptr; // 2014/8/22 21:54 自己都搞晕了 ???

		base=relationTabMeta.base;
		offset=relationTabMeta.laSymPtrMeta.offset;
		size=relationTabMeta.laSymPtrMeta.size;
		curLaSymBase=base+offset;
		laIndex=symMeta->laSymPtrIndex;
		_size=laIndex*sizeof(int *);

		if(size>=_size){

			curLaSymBase=curLaSymBase+_size;

			_ptr=(int **)curLaSymBase;

			*_ptr=fn;
			flag=1;

		}
	}

	free(symMeta);

	return flag;

}

int fastHook(char *name,void *fn){
	int status=1;
	int count=0;
	int flag=0;

	count=_dyld_image_count();
	for(int i=0;i<count;i++){
		flag=setLaSymPtr(i,name,fn);
		if(flag){ // 成功
			status=0;
			break;
		}
	}

	return status;
}






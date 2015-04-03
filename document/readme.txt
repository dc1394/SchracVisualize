﻿================================================================================
【 ソフト名 】SchracVisualize
【  作成者  】@dc1394
================================================================================

★これは何？
　「Schrac」の計算結果のアウトプットファイルを読み込み、可視化するコードです。実
　行には、以下が必要です。
　・DirectX End-User Runtimes (June 2010)
　（ http://www.microsoft.com/en-us/download/details.aspx?id=8109 から、
　　「directx_Jun2010_redist.exe」をダウンロードしてインストールしてください）。
　・Visual Studio 2013 の Visual C++ 再頒布可能パッケージ x86版
　（ https://www.microsoft.com/ja-jp/download/details.aspx?id=40784vcredist_x86.exe
　　から、「vcredist_x86.exe」をダウンロードしてインストールしてください）。

★使い方
　SchracVisualize.exeを実行すると、ファイル選択ダイアログが現れますので、可視化
　したい軌道のデータファイルを選択してください。

★更新履歴
　2015/03/21 ver.0.1  とりあえず公開。
　2015/03/31 ver.0.2　描画と計算を別スレッドで同時に行うようにした。

★ライセンス
　このソフトはフリーソフトウェアです（修正BSDライセンス）。
--------------------------------------------------------------------------------
　SchracVisualize
　Copyright (C) 2015 @dc1394

　1.ソースコード形式であれバイナリ形式であれ、変更の有無に関わらず、以下の条件を
　満たす限りにおいて、再配布および利用を許可します。

　1-1.ソースコード形式で再配布する場合、上記著作権表示、 本条件書および第2項の責
　任限定規定を必ず含めてください。
　1-2.バイナリ形式で再配布する場合、上記著作権表示、 本条件書および下記責任限定
　規定を、配布物とともに提供される文書 および/または 他の資料に必ず含めてくださ
　い。

　2.本ソフトウェアは無保証です。自己責任で使用してください。

　3.著作権者の名前を、広告や宣伝に勝手に使用しないでください。

  Copyright (c) 2015, @dc1394
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  * Neither the name of the <organization> nor the　names of its contributors 
    may be used to endorse or promote products derived from this software 
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------------

　SchracVisualizeにはMicrosoft社によるDXUTライブラリを使用しています。
  こちらのライセンスは以下になります。

/*
MICROSOFT SOFTWARE LICENSE TERMS
MICROSOFT DIRECTX SOFTWARE DEVELOPMENT KIT (SDK)
These license terms are an agreement between Microsoft Corporation (or based on where you live, one of its affiliates) and you.  Please read them.  They apply to the software named above, which includes the media on which you received it, if any.  The terms also apply to any Microsoft
•	updates,
•	supplements,
•	Internet-based services, and 
•	support services
for this software, unless other terms accompany those items.  If so, those terms apply.
BY USING THE SOFTWARE, YOU ACCEPT THESE TERMS.  IF YOU DO NOT ACCEPT THEM, DO NOT USE THE SOFTWARE.
If you comply with these license terms, you have the rights below.
1.	INSTALLATION AND USE RIGHTS.  
a.	Installation and Use.  You may install and use any number of copies of the software on your devices.
b.	Included Microsoft Programs.  The software contains other Microsoft programs.  The license terms with those programs apply to your use of them.
2.	ADDITIONAL LICENSING REQUIREMENTS AND/OR USE RIGHTS.
a.	Media Elements and Templates.  You may copy and use images, clip art, animations, sounds, music, shapes, video clips and templates provided with the software and identified for such use in documents and projects that you create.  You may distribute those documents and projects non-commercially.  If you wish to use these media elements or templates for any other purpose, go to www.microsoft.com/permission to learn whether that use is allowed.
b.	Distributable Code.  The software contains code that you are permitted to distribute in programs you develop if you comply with the terms below.
i.	Right to Use and Distribute.  The code and text files listed below are “Distributable Code.”
•	DIRECTX REDIST.TXT Files.  You may copy and distribute the object code form of code listed in DIRECTX REDIST.TXT files.
•	Sample Code.  You may modify, copy, and distribute the source and object code form of code marked as “sample”, as well as those marked as follows:
\Utilities\bin\x86\dxerr
\Utilities\bin\x64\dxerr
\Utilities\bin\x86\dxtex
\Utilities\bin\x64\dxtex
\Utilities\bin\x86\DxViewer
\Utilities\bin\x64\DxViewer
\Utilities\bin\x86\GDFTrace
\Utilities\bin\x64\GDFTrace
\Utilities\bin\x86\MeshConvert
\Utilities\bin\x64\MeshConvert
\Utilities\Source\Sas
\Utilities\Source\Effects11
•	Third Party Distribution.  You may permit distributors of your programs to copy and distribute the Distributable Code as part of those programs.
ii.	Distribution Requirements.  For any Distributable Code you distribute, you must
•	add significant primary functionality to it in your programs;
•	require distributors and external end users to agree to terms that protect it at least as much as this agreement; 
•	display your valid copyright notice on your programs; and
•	indemnify, defend, and hold harmless Microsoft from any claims, including attorneys’ fees, related to the distribution or use of your programs.
iii.	Distribution Restrictions.  You may not
•	alter any copyright, trademark or patent notice in the Distributable Code; 
•	use Microsoft’s trademarks in your programs’ names or in a way that suggests your programs come from or are endorsed by Microsoft; 
•	distribute Distributable Code to run on a platform other than the Windows, Xbox and Windows Mobile platforms;
•	include Distributable Code in malicious, deceptive or unlawful programs; or
•	modify or distribute the source code of any Distributable Code so that any part of it becomes subject to an Excluded License.  An Excluded License is one that requires, as a condition of use, modification or distribution, that
•	the code be disclosed or distributed in source code form; or 
•	others have the right to modify it.
3.	SCOPE OF LICENSE.  The software is licensed, not sold. This agreement only gives you some rights to use the software.  Microsoft reserves all other rights.  Unless applicable law gives you more rights despite this limitation, you may use the software only as expressly permitted in this agreement.  In doing so, you must comply with any technical limitations in the software that only allow you to use it in certain ways.    You may not
•	disclose the results of any benchmark tests of the software to any third party without Microsoft’s prior written approval;
•	work around any technical limitations in the software;
•	reverse engineer, decompile or disassemble the software, except and only to the extent that applicable law expressly permits, despite this limitation;
•	make more copies of the software than specified in this agreement or allowed by applicable law, despite this limitation;
•	publish the software for others to copy;
•	rent, lease or lend the software; or
•	use the software for commercial software hosting services.
4.	BACKUP COPY.  You may make one backup copy of the software.  You may use it only to reinstall the software.
5.	DOCUMENTATION.  Any person that has valid access to your computer or internal network may copy and use the documentation for your internal, reference purposes.
6.	EXPORT RESTRICTIONS.  The software is subject to United States export laws and regulations.  You must comply with all domestic and international export laws and regulations that apply to the software.  These laws include restrictions on destinations, end users and end use.  For additional information, see www.microsoft.com/exporting.
7.	SUPPORT SERVICES. Because this software is “as is,” we may not provide support services for it.
8.	ENTIRE AGREEMENT.  This agreement, and the terms for supplements, updates, Internet-based services and support services that you use, are the entire agreement for the software and support services.
9.	APPLICABLE LAW.
a.	United States.  If you acquired the software in the United States, Washington state law governs the interpretation of this agreement and applies to claims for breach of it, regardless of conflict of laws principles.  The laws of the state where you live govern all other claims, including claims under state consumer protection laws, unfair competition laws, and in tort.
b.	Outside the United States.  If you acquired the software in any other country, the laws of that country apply.
10.	LEGAL EFFECT.  This agreement describes certain legal rights.  You may have other rights under the laws of your country.  You may also have rights with respect to the party from whom you acquired the software.  This agreement does not change your rights under the laws of your country if the laws of your country do not permit it to do so.
11.	DISCLAIMER OF WARRANTY.   THE SOFTWARE IS LICENSED “AS-IS.”  YOU BEAR THE RISK OF USING IT.  MICROSOFT GIVES NO EXPRESS WARRANTIES, GUARANTEES OR CONDITIONS.  YOU MAY HAVE ADDITIONAL CONSUMER RIGHTS UNDER YOUR LOCAL LAWS WHICH THIS AGREEMENT CANNOT CHANGE.  TO THE EXTENT PERMITTED UNDER YOUR LOCAL LAWS, MICROSOFT EXCLUDES THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
12.	LIMITATION ON AND EXCLUSION OF REMEDIES AND DAMAGES.  YOU CAN RECOVER FROM MICROSOFT AND ITS SUPPLIERS ONLY DIRECT DAMAGES UP TO U.S. $5.00.  YOU CANNOT RECOVER ANY OTHER DAMAGES, INCLUDING CONSEQUENTIAL, LOST PROFITS, SPECIAL, INDIRECT OR INCIDENTAL DAMAGES.
This limitation applies to
•	anything related to the software, services, content (including code) on third party Internet sites, or third party programs; and
•	claims for breach of contract, breach of warranty, guarantee or condition, strict liability, negligence, or other tort to the extent permitted by applicable law.
It also applies even if Microsoft knew or should have known about the possibility of the damages.  The above limitation or exclusion may not apply to you because your country may not allow the exclusion or limitation of incidental, consequential or other damages.
Please note: As this software is distributed in Quebec, Canada, some of the clauses in this agreement are provided below in French.
Remarque : Ce logiciel étant distribué au Québec, Canada, certaines des clauses dans ce contrat sont fournies ci-dessous en français.
EXONÉRATION DE GARANTIE. Le logiciel visé par une licence est offert « tel quel ». Toute utilisation de ce logiciel est à votre seule risque et péril. Microsoft n’accorde aucune autre garantie expresse. Vous pouvez bénéficier de droits additionnels en vertu du droit local sur la protection des consommateurs, que ce contrat ne peut modifier. La ou elles sont permises par le droit locale, les garanties implicites de qualité marchande, d’adéquation à un usage particulier et d’absence de contrefaçon sont exclues.
LIMITATION DES DOMMAGES-INTÉRÊTS ET EXCLUSION DE RESPONSABILITÉ POUR LES DOMMAGES.  Vous pouvez obtenir de Microsoft et de ses fournisseurs une indemnisation en cas de dommages directs uniquement à hauteur de 5,00 $ US. Vous ne pouvez prétendre à aucune indemnisation pour les autres dommages, y compris les dommages spéciaux, indirects ou accessoires et pertes de bénéfices.
Cette limitation concerne :
•	tout  ce qui est relié au logiciel, aux services ou au contenu (y compris le code) figurant sur des sites Internet tiers ou dans des programmes tiers ; et
•	les réclamations au titre de violation de contrat ou de garantie, ou au titre de responsabilité stricte, de négligence ou d’une autre faute dans la limite autorisée par la loi en vigueur.
Elle s’applique également, même si Microsoft connaissait ou devrait connaître l’éventualité d’un tel dommage.  Si votre pays n’autorise pas l’exclusion ou la limitation de responsabilité pour les dommages indirects, accessoires ou de quelque nature que ce soit, il se peut que la limitation ou l’exclusion ci-dessus ne s’appliquera pas à votre égard.
EFFET JURIDIQUE.  Le présent contrat décrit certains droits juridiques. Vous pourriez avoir d’autres droits prévus par les lois de votre pays.  Le présent contrat ne modifie pas les droits que vous confèrent les lois de votre pays si celles-ci ne le permettent pas.
*/

  SchracVisualizeにはBoostコミュニティによるBoost C++ Librariesを使用しています。
  こちらのライセンスは Boostライセンス になります。

/* Boost Software License - Version 1.0 - August 17th, 2003
*
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:

* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

  SchracVisualizeにはGNU ProjectによるGNU Scientific Libraryを使用しています。こ
　ちらのライセンスは GNU General Public License になります。

  SchracVisualizeには®Intel CorpolationによるIntel® Threading Building Blocks
　(Intel® TBB)を使用しています。
  こちらのライセンスは GNU General Public License, version 2 になります。
